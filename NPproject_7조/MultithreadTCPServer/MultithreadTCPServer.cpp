#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

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

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE + 1];
    char ch[BUFSIZE + 1];

    CRITICAL_SECTION cs;

    InitializeCriticalSection(&cs); //Critical Section 초기화

    // 클라이언트 정보 얻기
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

    FILE* fp; ///
    
    fp = fopen("log.txt", "a+");
    if (!fp) {
        printf("파일 열기 실패 \n");
        WSACleanup();
        return 0;
    }
    else printf("파일 열기 성공\n");

    while (1) {
        // 데이터 받기
        retval = recv(client_sock, buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0) {
            break;
        }
        // 받은 데이터 출력
        buf[retval] = '\0';
        if (buf[0] == '!' && buf[1] == 'd') { //download
            printf("[TCP 서버] 다운로드 요청\n");
            EnterCriticalSection(&cs);
            fclose(fp);
            fp = fopen("log.txt", "r");
            fgets(buf, retval-1, fp); //명령어를 제외하고 입력
            fclose(fp);
            fp = fopen("log.txt", "a+");
            LeaveCriticalSection(&cs);
        } else if (buf[0] == '!' && buf[1] == 'v') { //view
            printf("[TCP 서버] 파일 내용 요청\n");
            EnterCriticalSection(&cs);
            fclose(fp);
            fp = fopen("log.txt", "r");
            fgets(buf, retval-1, fp); //명령어를 제외한 결과를 반환
            fclose(fp);
            fp = fopen("log.txt", "a+");
            LeaveCriticalSection(&cs);
        }  else if (buf[0] == '!' && buf[1] == 'o') { //overwrite
            printf("[TCP 서버] 덮어쓰기 요청\n");
            EnterCriticalSection(&cs);
            fclose(fp);
            fp = fopen("log.txt", "w"); //빈 파일로 덮어쓰는 동작
            strcpy(ch, buf + 2); //명령어(!o)를 제외하고 입력하기 위함
            fputs(ch, fp);
            fclose(fp);
            fp = fopen("log.txt", "a+");
            LeaveCriticalSection(&cs);
        }
        else if (buf[0] == '!' && buf[1] == 'h' || buf[0] == '!' && buf[1] == '?') { //help
            printf("[TCP서버] 명령어 도움말 요청\n");
        }
        else if (buf[0] == '!' && buf[1] == 'x') {
            printf("[TCP서버] 파일 삭제 동작\n");
        }
        else { //평문
            EnterCriticalSection(&cs);
            fputs(buf, fp); //파일에 입력
            fclose(fp);
            fp = fopen("log.txt", "a+");
            LeaveCriticalSection(&cs);
        }
        printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
            ntohs(clientaddr.sin_port), buf);

        // 데이터 보내기
        retval = send(client_sock, buf, retval, 0); //retval
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
    }

    // closesocket()
    closesocket(client_sock);
    printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
        inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    return 0;
}

int main(int argc, char *argv[])
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;
    HANDLE hThread;

    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // 접속한 클라이언트 정보 출력
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // 스레드 생성
        hThread = CreateThread(NULL, 0, ProcessClient,
            (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }

    // closesocket()
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}
