
# å®šä¹‰ANSIè½¬ä¹‰ç 
ESC_SEQ="\x1b["
RESET_SEQ="${ESC_SEQ}0m"
COLOR_SEQ="${ESC_SEQ}38;5;"

# å®šä¹‰é¢œè‰²ä»£ç 
COLOR_RED="${COLOR_SEQ}9m"
COLOR_GREEN="${COLOR_SEQ}10m"
COLOR_YELLOW="${COLOR_SEQ}11m"
COLOR_BLUE="${COLOR_SEQ}12m"

file1_base64="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"

# è¾“å‡ºå½©è‰²æ–‡æœ¬
if [[ -e /proc/uevents_records ]]; then
if grep -q 'entryi' /proc/uevents_records; then
  for i in $(seq 1 50)
do
    echo -e "${COLOR_RED}æ£€æµ‹åˆ°ä½ åˆ·å…¥äº†æ—§ç‰ˆæœ¬å†…æ ¸ï¼Œè¯·é‡å¯è®¾å¤‡åŽå†åˆ·å…¥æ–°çš„ï¼${RESET_SEQ}"
done
exit
fi
fi

echo -e "${COLOR_YELLOW}â†’ ä¸‹æ–¹å‡ºçŽ° Invalid argument å†è¯•ä¸€æ¬¡${RESET_SEQ}"
echo -e "${COLOR_YELLOW}â†’ OPPO Realme ä¸€åŠ  éœ€è¦è¿‡ç­¾åéªŒè¯ + å‡çº§åˆ°å®‰å“13${RESET_SEQ}"
echo -e "${COLOR_YELLOW}â†’ å¼€æœºä¸€æ®µæ—¶é—´åŽå¯èƒ½ä¼šåˆ·ä¸è¿›ï¼Œè‡ªåŠ¨é‡å¯åŽå†åˆ·ä¸€éå³å¯${RESET_SEQ}"
#echo
#[root@localhost ~]# cat test.sh
#!/bin/sh
#rm -rf /data/koyz

echo 0>/data/nh
echo -e "${COLOR_YELLOW}æ­£åœ¨æ£€æµ‹æ˜¯å¦å·²ç»åˆ·å…¥è¿‡ä¸€æ¬¡ ...${RESET_SEQ}"
echo
sleep 1.6
if [[ ! -e /data/nh ]]; then
echo -e "${COLOR_RED}æ— éœ€é‡å¤åˆ·å…¥ï¼æ¯æ¬¡å¼€æœºåˆ·ä¸€æ¬¡å°±è¡Œï¼Œå¦‚éœ€å‡çº§é©±åŠ¨è¯·å…ˆé‡å¯ã€‚${RESET_SEQ}"
exit
fi

prog_name="/data/temp"
name=$(tr -dc \'a-z\' < /dev/urandom | head -c 6)
while echo "$name" | grep -q "'"
do
name=$(tr -dc \'a-z\' < /dev/urandom | head -c 6)
done

sed "1,/^# END OF THE SCRIPT/d" "$0" > ${prog_name}   # å¯¼å‡ºäºŒè¿›åˆ¶ç¨‹åºï¼Œè¿™ä¸ªæ­¥éª¤å¾ˆé‡è¦ ...
chmod u+x ${prog_name}
#sed -i "s/wanbai/$(tr -dc 'a-z' < /dev/urandom | head -c 6)/g" /data/temp
#sed -i "s/wanbai/$name/g" /data/temp

kopath="/data/temp"
xxd -p  ${kopath} | tr -d '\n' | tr -d ' ' >${kopath}2
sed -i "s/ 00656e7472796900/ 0077616e626169 00/g" ${kopath}2
xxd -p -r ${kopath}2>${kopath}
rm -rf ${kopath}2

sed -i "s/wanbai/$name/g" /data/temp



#!/bin/bash


#å¡å¯†æ–‡ä»¶éªŒè¯
# èŽ·å– Android ç‰ˆæœ¬å·
insmod ${prog_name}
# && rm -f ${prog_name}
r=$?
echo
sleep 0.3
if [[ -e /dev/${name} ]]; then
rm -f ${prog_name}
    for i in $(seq 1 10)
do
    echo -e "${COLOR_GREEN}é©±åŠ¨åˆ·å…¥æˆåŠŸï¼${RESET_SEQ}"
    #echo -e "${COLOR_RED}åˆ·å…¥å¤±è´¥ï¼Œè¯·å°è¯•å…¶ä»–è„šæœ¬ã€‚${RESET_SEQ}"
done
echo $file1_base64 | base64 -d > temp
mv temp /data/$name
chmod 777 /data/$name
echo
echo -e "${COLOR_YELLOW}è„šæœ¬å¯ä»¥é€€å‡ºäº† ...${RESET_SEQ}"
dmesg -C
nohup /data/$name
else
echo -e "${COLOR_RED}åˆ·å…¥å¤±è´¥ï¼Œæ­£åœ¨è¿›è¡ŒäºŒæ¬¡å°è¯• ...${RESET_SEQ}"
echo
#å†è¯•ä¸€æ¬¡
CQ=0
if [ $r -eq 0 ]; then
CQ=1
fi

insmod ${prog_name} && rm -f ${prog_name}
r=$?
echo
sleep 0.3
if [[ -e /dev/${name} ]]; then
    for i in $(seq 1 10)
do
    echo -e "${COLOR_GREEN}é©±åŠ¨åˆ·å…¥æˆåŠŸï¼${RESET_SEQ}"
    #echo -e "${COLOR_RED}åˆ·å…¥å¤±è´¥ï¼Œè¯·å°è¯•å…¶ä»–è„šæœ¬ã€‚${RESET_SEQ}"
done
echo $file1_base64 | base64 -d > temp
mv temp /data/$name
chmod 777 /data/$name
echo
echo -e "${COLOR_YELLOW}è„šæœ¬å¯ä»¥é€€å‡ºäº† ...${RESET_SEQ}"
dmesg -C
nohup /data/$name
fi

   for i in $(seq 1 10)
do
    #echo -e "${COLOR_GREEN}é©±åŠ¨åˆ·å…¥æˆåŠŸï¼${RESET_SEQ}"
    echo -e "${COLOR_RED}åˆ·å…¥å¤±è´¥ï¼Œè¯·é‡å¯æ‰‹æœºåŽå†è¯•ä¸€æ¬¡ï¼Œç¡®å®šä¸è¡Œå†æ¢å…¶ä»–è„šæœ¬ã€‚${RESET_SEQ}"
#    echo -e "${COLOR_YELLOW}å¦‚æžœä¸Šæ–¹æ²¡æœ‰æŠ¥é”™è¾“å‡ºï¼Œè¯·é‡å¯æ‰‹æœºåŽå†å°è¯•å…¶ä»–è„šæœ¬ï¼Œå¦åˆ™å¯èƒ½ä¼šå µå¡žæŽ¥å£å¯¼è‡´æœ¬è¯¥æˆåŠŸçš„ä¹Ÿéƒ½ä¾ä¾å˜æˆäº†å¤±è´¥ã€‚${RESET_SEQ}"
done

if [ CQ -eq 1 ]; then
    #echo "result ç­‰äºŽ 0"
    echo
    echo 3ç§’åŽè‡ªåŠ¨é‡å¯è®¾å¤‡ ...
    sleep 3
    reboot
fi
    
fi

rm -rf /data/koyz
rm -rf /data/temp


# WARNING: Do not modify the following !!!
exit 0
# END OF THE SCRIPT ----------> è¿™æ˜¯shell è„šæœ¬å½“å‰çš„æœ€åŽä¸€è¡Œ
ELF          ·                    ¸9          @     @ !                  (˜^Ó	$@ù*yhø
6)yhø*üRÓ  @ùJ!}’)t’)
ª)Ë*@ùÊ ´)@ù*üIÓJ!}’HË)t’iiø) €Ò	€àò	ê€  T t’ ,@³À_ÖàªÀ_Ö Õ Õ Õ^† øý{¾©ôO©ý ‘óªôª   ”  ´@A9h  7à*  @ùi‹?ëà‡ŸôOA©ý{Â¨^Ž_øÀ_Ö^† øý{½©õ ùõ ª üLÓôO©ý ‘óªôª   ”  4  @ùµë` Thþ_Óè µàªáª" €R   ”A8ÕÙ@9
@ù‰Þ@“‹ (7@ùëªL Ð6ë	ªk«êƒŠšk1ŸÚ
úë‡Ÿšk ´ Õ@ù?(ê€ŸšŸ"Õáªâª   ”ó ª Õ ñàŸ  à*ôOB©õ@ùý{Ã¨^Ž_øÀ_Ö3 €R  !Ôõÿÿ Õ Õ^† øý{½©ôO©ô ª üLÓõ ùý ‘óªõª   ”  4  @ù”ë` Thþ_ÓH µàªáªâ*   ”A8ÕÙ@9
@ù©Þ@“‹ (7@ùëªL Ð6ë	ªâªk«êƒŠšk1ŸÚ
úë‡Ÿšk ´ Õ@ù?(ê¡ŸšŸ"Õàªâª   ”â ª ÕÂ  µ  €R	  à*  !Ô  hËá*€‹   ”à*ôOB©õ@ùý{Ã¨^Ž_øÀ_Ö Õ^† øý{½©öW©ý ‘ôO©óªôªõª   ”` ´á*   ”  ´   ”À ´áªö ª   ”à  ´@A9¨  6@ùi‹?ëI Tàª   ”à*ôOB©   öWA©ý{Ã¨^Ž_øÀ_Öàª   ”¨š^ÓÉ&@ù*yhøŠþ6)yhøªþRÓ  @ùJ!}’)t’)
ª)Ë*@ùJýÿ´)@ùªþIÓJ!}’HË)t’iiø) €Ò	€àò	ê üÿT t’ .@³Àûÿ´áªâª   ”Úÿÿ Õ^† øý{½©öW©ý ‘ôO©óªôªõª   ”` ´á*   ”  ´   ”À ´áªö ª   ”à  ´@A9¨  6@ùi‹?ëI Tàª   ”à*ôOB©   öWA©ý{Ã¨^Ž_øÀ_Öàª   ”¨š^ÓÉ&@ù*yhøŠþ6)yhøªþRÓ  @ùJ!}’)t’)
ª)Ë*@ùJýÿ´)@ùªþIÓJ!}’HË)t’iiø) €Ò	€àò	ê üÿT t’ .@³Àûÿ´áªâª   ”Úÿÿ ÕÿÑ^† øý{©ýC‘üW©ôO©A8Õ-Cùóª¨ƒø   ”€ ´á*   ”  ´   ”à ´ô ª   ”•@ùµ  µàª!  µ
@ùµÿÿ´ÿÿ©ÿÿ©ÿÿ©ÿÿ©ÿÿ©ÿÿ
© R@ùÿÿ	©ÿÿ©ÿÿ©ÿÿ©ÿÿ©ÿÿ©ÿÿ©ÿÿ©ÿÿ©ÿÿ © ýÿ´á# ‘â€R   ”á€Rô ª   ”  ñáª€€š   ”@üÿ5 @ùA8Õ-Cù©ƒ_ø	ëá  TôOS©üWR©ý{Q©^Ž_øÿ‘À_Ö   ” Õà*À_Ö Õ Õà*À_Ö Õ Õ^† øý{½©( QöW© qôO©ý ‘( T	  óª) ‘
  +y¨¸J‹@ÖA8Õ
Ù@9	@ùŠ (7@ùêªK Ð6jÞ@“J ±éƒ‰šJ1ŸÚ_	úê‡Ÿšj ´ Õ@ùiÞ@“?(êaŸš  Ÿ"Õs ‘€Ràª   ” Õ  µaŠ@©`@¹c@ù   ”` 7  ?$q  T?$qÁ T  `@ù  @¹   ”`@ù   ”¦  A8Õ
Ù@9	@ùŠ (7@ùêªK Ð6jÞ@“J ±éƒ‰šJ1ŸÚ_	úê‡ŸšÊ ´ Õ@ùiÞ@“?(êaŸš  Ÿ"Õs ‘€Ràª   ” Õ` µaŠ@©`@¹c@ù   ” 
 6…  A8Õ©Ú@9¨@ù‰ (7ª@ùéªJ Ð6iÞ@“)a ±èƒˆš)1ŸÚ?úé‡Ÿš© ´   Õ” ‘¨@ùvÞ@“ß(êaŸšŸ"Õàª€R   ” Õ@ µ  @ù«Ú@9ª@ù	Ý@“‹ (7¬@ùëªL Ð6ë	ªký±êƒŠšk1ŸÚ
úë‡Ÿš ´    Õª@ù?*êŸšŸ"Õ   ‘â€R   ” Õà µ  €@¹!  ‘   ”€
 ù©Ú@9¨@ù‰ (7ª@ùéªJ Ð6éª)a ±èƒˆš)1ŸÚ?úé‡Ÿš‰ ´   Õ¨@ùß(ê`ŸšŸ"Õ!  ‘€R   ” Õ@ ´  €’1  A8ÕÙ@9
@ùiÞ@“‹ (7@ùëªL Ð6ë	ªk ±êƒŠšk1ŸÚ
úë‡Ÿšë ´    Õ@ù?(êaŸšŸ"Õ   ‘€R   ” ÕÀ µHS€R@ €R	  ( ¹            ‘!  ‘B  ‘   ”    @¹„  ‘áªãª    ù   ”àªôOB©öWA©ý{Ã¨^Ž_øÀ_Ö€R  â ª   ‘Ë  ‘  €R  â ª   ‘Ë a ‘  â€R  â ª   ‘Ë ý‘á*   ”  €’áÿÿ        
         
         
         
         
         
         
         
         
         
         
         
         
         
         
         
 Ÿ@ ÕŸA ÕŸ@ ÕŸA ÕŸ@ ÕŸA ÕŸ@ ÕŸA ÕŸ@ ÕŸA ÕŸ@ ÕŸA ÕŸ@ ÕŸA ÕŸ@ ÕŸA Õ        £ 	        £ 	include/linux/thread_info.h    Ä   H  X  X  X  X  œ  ÿÑ^† øý{©ýC ‘öW©ôO©A8Õ-Cù³ €Rè ùÿ ¹   ”s qÁÿÿTà ‘ €R   ”è€¹éÌŒRÉÌ¬rK€R  	}	›s ‘*ýÓ)ýb“)
)¡( ? 1è ¹ë Tõ‰Rôª•Ø©rV€R   ”	|µ›è€¹)ýcÓ))…ij48” ‘ŸëëþÿT        ‘c  ‘á*" €RÊ(8   ”`ø7    s ‘!  ‘àª   ”    @¹ ‘àª" €Rh2 ù   ”Àø7          ‘!  ‘B  ‘   ”  ` ùü?± T      ‘   ”€@¹! €R   ”s@¹&  ó *$  ó *€@¹! €R   ”    ‚@¹„  ‘áªãª   ”  s ‘àª   ”€  6i"@©( ù	 ù`"‘s ùs ù   ”   áª   ‘   ”   áª   ‘   ”€@¹! €R   ”ó*A8Õ-Cùé@ù	ë Tà*ôOC©öWB©ý{A©^Ž_øÿ‘À_Ö   ”^† øý{¾©ôO©  `@ù  @¹ý ‘   ”`@ù   ”      ‘   ”€@¹! €R   ”ôOA©ý{Â¨^Ž_øÀ_Ö    wanbai sched_debug uevents_records                                                                                                                                                                                                                                                                                                      description=wanbai license=GPL author=wanbai vermagic=5.4.237-VK-Y SMP preempt mod_unload modversions aarch64 name=entryi depends=           Linux                                                           entryi                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           GNU 6É_³½R‰         X                    X           ˜         Z           ô         \           ü         X                    X                    ]           |        ^           ä        \           ì        X           ð        X                   ]           p        `                    a           à        c           ì        d           ô        e                   Z           (        f           L        f           h        X           l        X           Ä        [           ð        c           ü        d                   e                   Z           8        f           \        f           x        X           |        X           Ô        _                   c                   d                   e           (        f           ”        i                    j           °        k           è        l           0        A           8        A           ˜        D                    D           ¬        `           Ä        b           à        D   P      ä        D   P      è        D   X      ì        D   X      ð        p           ô        D   P      ø        q           L        D           T        D           `        `           x        g           ¼        D           Ä        D           ä        `           ð        D   (       ô        D   (       0        D   8       H        D   8       P        `           \        D   8       d        D   8       h        h           ¤        D           ¼        D           Ä        ^           	        D           ,	        D           4	        `           H	        D           L	        D           T	        r           X	        D   9      \	        D   8      `	        r           d	        D   9      h	        D   8      l	        s           p	        U           t	        D   X      x	        D   X      |	        U           ˆ	        D   P      Œ	        D   P      	        t           ¸	        D           ¼	        D           Ø	        D           Ü	        D           ø	        D   8       ü	        D   8       
        a                     =   `               ?                    =   „               ?                   =   T               ?          $         =   x      (         ?          0         =   „      4         ?          <         =   °      @         ?          H         =   8      L         ?          T         =   d      X         ?          `         =   À      d         ?           l         =   è      p         ?   $       x         =   4      |         ?   (       „         =   T      ˆ         ?   ,                =   ¨      ”         ?   0       œ         =   È                ?   4       ¨         =   	      ¬         ?   8       ´         =   8	      ¸         ?   <                 =   °                                  =   Œ                         ,         v           @         w           T         D   9      \         D   9               v           ¸         D   X      ¼         U           À         D   X      Ä         U           Ô         x           Ü         D   `      à         y           ä         D   `      è         y           ð         z           ô         r           ø         D   X      ü         D   X               r                   {                   r                   D   9               D   è      $        r           (        D   9      ,        D   è      0        s           4        D   P      8        D   P      D        D   `      H        D   `      L        |           P        D   X      X        }           \        D   P      p        D   X      x        }           €        U           „        D   X      ˆ        U           ”        t           ˜        r          œ        r          ¤        ~           Ä                   È        U          Ð        U          Ô        €           Ø        U          à        U          ä        €           è        D   X      ð        }           (        l                    D   P               D   P               D   X               D   X                p           $         D   P      (         q           ,         D   `      0         D   `      4         |           8         D   X      @         }                     r           P         o           p         m           €         n           Ubuntu clang version 14.0.0-1ubuntu1.1  YZ[$2^1e>@A67	 p        u           8                                           ×                     ²                     p                     #                    ï                    ·    	                 É                     ƒ                     N     °              7                                         Ñ     $               ¬                    ž                     j                          Œ              é     0               ±                    ¹                    }     <               H                    ÷     P             `    X             1     H                                   Ã     T               ¦                    ó                   d     `                                    ã     l               «     $               g    8              w     x               B     (               +     „                    ,               ½                    ˜     0               ^     œ                    4               Ý     ¨               ¥     8               q     ´               <     <               (    9             l     8             %                                               `      ˆ       Y     è             ·                     z                     5                     ô                     ¼                   ×                     ˆ                  [                  Ÿ     '                                                                                      	                                                                                                                                                                           S                   Ü                     €    -       A       ²     -               J    n              u                                         ú    z       	       #     '                                                                                                                                                   ±             t       ¡                      Û    €       P                            á     Ð       è       È                                          6                     Ê     À      ü       J                     š                      (     À            ¤                                           ­                     Š                           Ð            W    à                                 .                     k                     Ú                     Ÿ    ð             H                  Ë                J                      <                      ‡            €      +                     :                     {            ,      ;                                          ‹                                                              Ò                     ë                     r                     ±                     ô                                           •            T        .note.Linux .rela.exit.text .rela.init.text .rela.text .comment .altinstr_replacement .init.plt .bss .rela.altinstructions __versions .rodata.str .modinfo .note.GNU-stack .llvm_addrsig .text.ftrace_trampoline .rela.gnu.linkonce.this_module .rela__bug_table .note.gnu.build-id .shstrtab .strtab .symtab .rodata .rela.data .rodata.str1.1  remove_proc_entry write_process_memory read_process_memory class_destroy device_destroy driver_entry.__key dispatch_ioctl.__key char_dev mmput cdev_init memset physvirt_offset translate_linear_address write_physical_address read_physical_address char_class dispatch_fops ____versions get_random_bytes strrchr __arch_copy_to_user __arch_copy_from_user dev_number strcmp unregister_chrdev_region alloc_chrdev_region dispatch_open get_task_mm dispatch_ioctl.cm dispatch_ioctl __stack_chk_fail cdev_del kobject_del get_pid_task file_path __check_object_size __class_create device_create dispatch_close get_module_base dispatch_ioctl.name init_module __this_module cleanup_module find_get_pid __list_del_entry_valid pfn_valid cdev_add check_proc_map_can_read dispatch_ioctl.mb find_vma $x.39 $x.29 $x.19 $x.9 $d.38 $d.28 $d.18 $d.8 $x.37 $x.27 $x.17 $x.7 _note_6 __UNIQUE_ID_author176 $d.36 $d.26 $d.16 $d.6 __UNIQUE_ID_license175 $d.45 $x.35 $x.25 $x.15 $d.5 __UNIQUE_ID_description174 $d.44 $d.34 $d.24 $d.14 $x.4 $d.43 __UNIQUE_ID_depends133 $x.33 $x.23 $x.13 $d.3 DEVICE_NAME2 $d.42 get_random_u32 __UNIQUE_ID_name132 $d.32 $d.22 $d.12 $x.2 $d.2 $d.41 __UNIQUE_ID_vermagic131 $x.31 .Ltmp21 $x.21 $x.11 $d.1 $x.40 $d.30 $d.20 .Ltmp10 $d.10 $x.0 $d.0                                                                       \                     @                                     W                     A                                     º                     B                                     2                     P       
                             -      @               Ø      	                          k                     h
      À                              f      @               ð                                 A                     (      @                              ö                     h                                    ñ      @               ð      `          	                 ‡      2               €                                   /                    œ                                     "                     ¼      ,                                   @               P                                                      è      T                                    @               `%                                 a                     @      é                             B     2               @      #                             <                    h                                    7     @               €&      `                           “                     ˆ      ƒ                              8      0               à&      (                             ¬   Lÿo   €            '                                                                                             ×                     @      €              @               Ò      @                '      0                           |                     À                                                         À                                    œ                      P'                                     '                     P'      0          W                                      €3      Q                                                  Ñ4      á                             