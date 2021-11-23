#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9152
#define BUFSIZE    512

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char *)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
    int received;
    char *ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
}

int main(int argc, char *argv[])
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // connect()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");

    // 데이터 통신에 사용할 변수
    char buf[BUFSIZE + 1];
    int len;

    FILE* fp; ///
    char fb[BUFSIZE + 1] = {0};

    // 서버와 데이터 통신
    while (1) {
        // 데이터 입력
        printf("\n[보낼 데이터] ");
        if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
            break;

        // '\n' 문자 제거
        len = strlen(buf);
        if (buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        if (strlen(buf) == 0)
            break;

        // 데이터 보내기
        retval = send(sock, buf, strlen(buf), 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        } else if (buf[0] == '!' && buf[1] == 'd') { //download
            fb[0] = buf[0];
            fb[1] = buf[1];
            fp = fopen("logcpy.txt", "a+");
            printf("[TCP 클라이언트] %d바이트 파일 다운로드 요청\n", retval);
            if (!fp) {
                printf("파일열기 실패 \n");
                WSACleanup();
                return 0;
            }
            else printf("파일열기 성공\n");
        } else if (buf[0] == '!' && buf[1] == 'v') { //view
            fb[0] = buf[0];
            fb[1] = buf[1];
            printf("[TCP 클라이언트] %d바이트 파일 보기 요청\n", retval);
        } else if (buf[0] == '!' && buf[1] == 'o') { //overwrite
            fb[0] = buf[0];
            fb[1] = buf[1];
            printf("[TCP 클라이언트] %d바이트 덮어쓰기 요청\n", retval);
        }
        else if (buf[0] == '!' && buf[1] == 'h' || buf[0] == '!' && buf[1] == '?') { //도움말
            printf("[TCP 클라이언트] 도움말\n");
            printf("1. 평문을 입력하면 해당 내용이 서버 측 텍스트 파일(log.txt)에 저장됩니다.\n");
            printf("2. 해당 파일은 !d 명령어로 다운로드가 가능합니다. logcpy.txt 파일로 저장됩니다.\n");
            printf("3. 해당 파일의 내용은 !v로 확인하실 수 있습니다. 파일에 저장된 내용이 없는 경우 보낸 메시지를 리턴합니다.\n");
            printf("4. !o를 통해 내용 덮어쓰기가 가능합니다. 뒤에 아무 것도 입력하지 않으면 모든 내용을 삭제합니다. \n");
            printf("5. !x를 통해 logcpy.txt 내의 내용을 삭제할 수 있습니다.\n");
            printf("아무 것도 입력하지 않으면 프로그램을 종료합니다.\n");
            printf("!x를 제외한 모든 명령어는 뒤에 문자열을 입력해야 합니다. 해당 바이트 수 만큼의 데이터를 입출력합니다.\n");
            printf("평문을 제외한 명령어 입력은 텍스트 파일에 저장되지 않습니다.\n");
            printf("!h 또는 !? 명령어를 통해 이 도움말을 다시 볼 수 있습니다.\n");
        }else if (buf[0] == '!' && buf[1] == 'x') { //delete
            fb[0] = buf[0];
            fb[1] = buf[1];
            fp = fopen("logcpy.txt", "w");
            printf("[TCP 클라이언트] 저장된 파일 내용 삭제\n");
            if (!fp) {
                printf("파일열기 실패 \n");
                WSACleanup();
                return 0;
            }
            else printf("파일열기 성공\n");
        }
        printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

        // 데이터 받기
        retval = recvn(sock, buf, retval, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // 받은 데이터 출력
        buf[retval] = '\0';
        if (fb[0] == '!' && fb[1] == 'd') { //다운로드
            fp = fopen("logcpy.txt", "w");
            if (!fp) {
                printf("파일열기 실패 \n");
                WSACleanup();
                return 0;
            } 
            else {
                printf("파일열기 성공\n");
                fputs(buf, fp); //파일에 입력
                printf("[TCP 클라이언트] 다운로드 완료\n");
                fclose(fp);
            }
        } else if (fb[0] == '!' && fb[1] == 'v') { //내용 보기
            printf("내용 보기\n");
        }
        else if (fb[0] == '!' && fb[1] == 'o') { //덮어쓰기
            printf("덮어쓰기\n");
        }
        else if (fb[0] == '!' && fb[1] == 'x') { //delete
            fp = fopen("logcpy.txt", "w");
            if (!fp) {
                printf("파일열기 실패 \n");
                WSACleanup();
                return 0;
            }
            else {
                printf("파일열기 성공\n");
                printf("[TCP 클라이언트] 삭제 완료\n");
                fclose(fp);
            }
        }
        printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
        printf("[받은 데이터] %s\n", buf);
    }

    // closesocket()
    closesocket(sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}
