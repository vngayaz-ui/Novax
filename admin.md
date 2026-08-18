# NovaX License Server — Admin Guide

Complete documentation for the Cloudflare Worker license server used by the
NovaX binary + APK.

---

## Product & Worker URL

| | |
|---|---|
| **Worker URL** | `https://novax-license.vngayaz1.workers.dev` |
| **API base (used by binary/APK)** | same URL (binary XOR-decodes it) |
| **Admin Secret** | `novax-admin-2024` |
| **NOVAX_SECRET** (anti-spoof token MD5 salt) | `Nx7Kp2Rv9mLwQ4tY8jFh3cBn6sXd5gZe` |
| **NOVAX_HMAC_KEY** (request signing) | `Kx9Rm3Pv7LwQ4tY8` |
| **Default MAX_DEVICES** | `1` (per-key unless overridden) |
| **Timestamp tolerance (anti-replay)** | `300` seconds (5 min) |
| **Nonce cache expiry** | 10 minutes |
| **Storage** | Cloudflare KV namespace `LICENSES` |
| **Request limit** | 100K requests/day (free tier) |

All admin routes require the header:

```
Authorization: Bearer novax-admin-2024
```

Public routes (`/activate`, `/validate`, `/deactivate`) need the HMAC-signed
payload described later.

---

## 1. Generate a License Key (ADMIN)

**Endpoint:** `POST /create`

**Headers:**
```
Content-Type: application/json
Authorization: Bearer novax-admin-2024
```

**Body:**
```json
{
  "username": "ayaz",
  "expiry_days": 365,
  "max_devices": 200
}
```

| Field | Required | Description |
|---|---|---|
| `username` | ✅ | Display name bound to the key |
| `expiry_days` | optional | Key lifetime in days. Omit / `null` = **lifetime** |
| `max_devices` | optional | Max devices that can bind. Defaults to `1` |

**Response:**
```json
{
  "success": true,
  "key": "H4P9-6JEJ-A90V-DMQP",
  "username": "ayaz"
}
```

> Keys are auto-generated in the format `XXXX-XXXX-XXXX-XXXX` (16 chars).
> An expired key stays usable via `max_devices` only if expiry is null.

---

## 2. List All Licenses (ADMIN)

**Endpoint:** `POST /list`

**Headers:**
```
Authorization: Bearer novax-admin-2024
```

**Body:** `{}` (or empty)

**Response:**
```json
{
  "success": true,
  "count": 2,
  "licenses": [
    {
      "key": "H4P9-6JEJ-A90V-DMQP",
      "username": "ayaz",
      "created": 1720000000000,
      "expiry": 1818542372272,
      "max_devices": 200,
      "devices": [
        { "device_id": "3B66240024900000-CPH2769-OnePlus", "activated_at": 1720000000000 }
      ],
      "banned": false
    }
  ]
}
```

---

## 3. Delete a License (ADMIN)

**Endpoint:** `POST /delete`

**Headers:**
```
Authorization: Bearer novax-admin-2024
```

**Body:**
```json
{
  "key": "H4P9-6JEJ-A90V-DMQP"
}
```

**Response:**
```json
{ "success": true, "message": "Deleted" }
```

> Deleting a key also removes the `user:<username>` mapping. The user's device
> must use a new key afterward.

---

## 4. Activate a Device (PUBLIC — used by binary)

**Endpoint:** `POST /activate`

Binds a `device_id` to a license on first install. If already bound, it just
returns a fresh token ("Already activated").

**Body (HMAC-signed):**
```json
{
  "key": "H4P9-6JEJ-A90V-DMQP",
  "device_id": "3B66240024900000-CPH2769-OnePlus",
  "timestamp": 1720000000,
  "nonce": "a3f1c9d2e5b68740",
  "hmac": "<hex md5 hmac>"
}
```

**HMAC computation (HMAC-MD5):**
```
sign_data = KEY.toUpperCase() + device_id + timestamp + nonce
hmac      = HMAC_MD5( sign_data, NOVAX_HMAC_KEY )   // "Kx9Rm3Pv7LwQ4tY8"
```

**Response (success):**
```json
{
  "success": true,
  "message": "Activated",
  "username": "ayaz",
  "expiry": 1818542372272,
  "token": "f2802eba0356edb959a8b02f6318988f"
}
```

`expiry` is JS epoch **milliseconds**. `token` = MD5(`NOVAX-key-hwid-NOVAX_SECRET`),
verified client-side to detect spoofed servers.

Error cases: missing fields → 400, HMAC/timestamp/nonce fail → 403
(`HMAC mismatch`, `timestamp expired`, `nonce replay detected`), license not
found → 404, banned/expired/device-limit → 403.

---

## 5. Validate a License (PUBLIC — APK uses this)

**Endpoint:** `POST /validate`

Read-only check: returns whether the given `device_id` is already activated
for the key. Does NOT bind a new device.

**Body:** same HMAC-signed payload as `/activate` (above).

**Response when device IS activated:**
```json
{
  "success": true,
  "valid": true,
  "username": "ayaz",
  "expiry": 1818542372272,
  "token": "f2802eba0356edb959a8b02f6318988f"
}
```

**Response when device is NOT activated:** `"valid": false`, `"token": ""`.

---

## 6. Deactivate a Device (PUBLIC)

**Endpoint:** `POST /deactivate`

Removes a `device_id` from the license so the slot frees up.

**Body:**
```json
{
  "key": "H4P9-6JEJ-A90V-DMQP",
  "device_id": "3B66240024900000-CPH2769-OnePlus"
}
```

**Response:**
```json
{ "success": true, "message": "Deactivated" }
```

> `/deactivate` does NOT do the HMAC/timestamp/nonce check — only `/activate`
> and `/validate` enforce it.

---

## Example: Create a 30-day key with a single device

```bash
curl -X POST https://novax-license.vngayaz1.workers.dev/create \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer novax-admin-2024" \
  -d '{"username":"thuggy","expiry_days":30,"max_devices":3}'
```

Returns:
```json
{ "success": true, "key": "A1B2-C3D4-E5F6-G7H8", "username": "thuggy" }
```

---

## How the NovaX binary uses this

1. **License init** — reads encrypted file `/data/local/tmp/.novax_license`,
   loads `H4P9-6JEJ-A90V-DMQP`.
2. **`BuildSignedPayload(key, device_id)`** builds `{key, device_id,
   timestamp, nonce, hmac}` using HMAC-MD5 with `NOVAX_HMAC_KEY`.
3. Binary calls **`/activate`** → binds the device → server returns
   `username`, `expiry`, anti-spoof `token`.
4. Binary verifies `token` locally (MD5 over
   `"NOVAX" + "-" + key + "-" + hwid + "-" + NOVAX_SECRET`).
5. If valid → `isLoggedIn = true` → the ImGui menu + ESP overlay unlock.

For the APK, the binary writes the decoded URL to `/data/local/tmp/.novax_api`
so the APK never hardcodes it (with a fallback random-looking XOR string).

---

## Editing / Deploying the Worker

- **Source:** `novax-license/src/index.js`
- **Deploy tool:** `wrangler`
  ```bash
  npx wrangler deploy
  ```
- **KV namespace (production):** `LICENSES`
- Set env vars if not using defaults:
  ```bash
  npx wrangler secret put ADMIN_SECRET
  npx wrangler secret put NOVAX_SECRET
  npx wrangler secret put NOVAX_HMAC_KEY
  ```