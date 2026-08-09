#pragma once
#ifndef MEMBER_H
#define MENBER_H
struct 二维 {
float x;
float y;
二维() {
this->x = 0;
this->y = 0;
}
二维(float x, float y) {
this->x = x;
this->y = y;
}
};
struct 三维 {
float x;
float y;
float z;
三维() {
this->x = 0;
this->y = 0;
this->z = 0;
}
三维(float x, float y, float z) {
this->x = x;
this->y = y;
this->z = z;
}
};
//四维成员变量
struct 四维{
float l;
float r;
float t;
float b;
};
struct FMatrix
{
float M[4][4];
};
//和四维一样
struct 缩放
{
float x;
float y;
float z;
float w;
};
struct FTransform
{
缩放 Rotation;
三维 Translation;
float chunk;
三维 Scale3D;
};
struct BoneStruct {
三维 Pos;
二维 ScreenPos;
};
//字符转换
typedef char UTF8;
typedef unsigned short UTF16;
//骨骼计算 使用内联关键字告诉编译器火速给我把代码直接放过去,而不是调用
inline void Get_Bone(三维 obj, float matrix[16],二维 Screen,二维 *Coords)
{
float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
Coords->x = (Screen.x / 2) + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * (Screen.x / 2);
Coords->y = (Screen.y / 2) - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * (Screen.y / 2);
}
inline 三维 MarixToVector(FMatrix matrix)
{
return 三维(matrix.M[3][0], matrix.M[3][1], matrix.M[3][2]);
}
inline FMatrix MatrixMulti(FMatrix m1, FMatrix m2)
{
FMatrix matrix = FMatrix();
for (int i = 0; i < 4; i++)
{
for (int j = 0; j < 4; j++)
{
for (int k = 0; k < 4; k++)
{
matrix.M[i][j] += m1.M[i][k] * m2.M[k][j];
}
}
}
return matrix;
}
inline FMatrix TransformToMatrix(FTransform transform)
{
FMatrix matrix;
matrix.M[3][0] = transform.Translation.x;
matrix.M[3][1] = transform.Translation.y;
matrix.M[3][2] = transform.Translation.z;
float x2 = transform.Rotation.x + transform.Rotation.x;
float y2 = transform.Rotation.y + transform.Rotation.y;
float z2 = transform.Rotation.z + transform.Rotation.z;
float xx2 = transform.Rotation.x * x2;
float yy2 = transform.Rotation.y * y2;
float zz2 = transform.Rotation.z * z2;
matrix.M[0][0] = (1 - (yy2 + zz2)) * transform.Scale3D.x;
matrix.M[1][1] = (1 - (xx2 + zz2)) * transform.Scale3D.y;
matrix.M[2][2] = (1 - (xx2 + yy2)) * transform.Scale3D.z;
float yz2 = transform.Rotation.y * z2;
float wx2 = transform.Rotation.w * x2;
matrix.M[2][1] = (yz2 - wx2) * transform.Scale3D.z;
matrix.M[1][2] = (yz2 + wx2) * transform.Scale3D.y;
float xy2 = transform.Rotation.x * y2;
float wz2 = transform.Rotation.w * z2;
matrix.M[1][0] = (xy2 - wz2) * transform.Scale3D.y;
matrix.M[0][1] = (xy2 + wz2) * transform.Scale3D.x;
float xz2 = transform.Rotation.x * z2;
float wy2 = transform.Rotation.w * y2;
matrix.M[2][0] = (xz2 + wy2) * transform.Scale3D.z;
matrix.M[0][2] = (xz2 - wy2) * transform.Scale3D.x;
matrix.M[0][3] = 0;
matrix.M[1][3] = 0;
matrix.M[2][3] = 0;
matrix.M[3][3] = 1;
return matrix;
}
#endif
