#include <bits/stdc++.h>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define DEFAULT_BUFLEN  1073741824
#define INVALID_SOCKET  0
#define SOCKET_ERROR   -1
#define MAXN 40000

int x[MAXN + 5];
multiset<int> s;
multiset<int>::iterator it;

int get_result(int, int);

class Client {
    int ConnectSocket;
    char *sendbuf, *recvbuf;
    sockaddr_in server;

    public:
        char* hostName;

        Client( char *, int );

        char* getLocalIP();
        char* getServerIP();

        void socketClient();
        void connectServer();
        void send_recv();
};

Client::Client( char *IP, int PORT ) {
    memset( &server, 0, sizeof( sockaddr_in ) );
    server.sin_addr.s_addr = inet_addr( IP );
    server.sin_family = AF_INET;
    server.sin_port = htons( PORT );

    sendbuf = new char[DEFAULT_BUFLEN];
    recvbuf = new char[DEFAULT_BUFLEN];
    hostName = new char[256];

    gethostname( hostName, 256 );
}

char* Client::getLocalIP() {
    hostent *localHost = gethostbyname( hostName );
    return inet_ntoa( *(struct in_addr*)*localHost->h_addr_list );
}
char* Client::getServerIP() {
    return inet_ntoa( server.sin_addr );
}

void Client::socketClient() {
    ConnectSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if( ConnectSocket == INVALID_SOCKET ) {
        printf( "Socket failed\n" );
        exit( -2 );
    }
}
void Client::connectServer() {
    int iResult = connect( ConnectSocket, ( sockaddr* )&server, sizeof( sockaddr_in ) );
    if( iResult == SOCKET_ERROR ) {
        close( ConnectSocket );
        ConnectSocket = INVALID_SOCKET;
    }

    if( ConnectSocket == INVALID_SOCKET ) {
        cout << "Unable to connect to server!\n";
        exit( -3 );
    }
}
void Client::send_recv() {
    int recvResult, sendResult;

    do {
        memset( recvbuf, 0, DEFAULT_BUFLEN );
        cout << ">>> waiting ..." << endl;
        recvResult = recv( ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0 );

        if( recvResult > 0 ) {
            cout << "recvbuf[" << recvResult << "]" << endl;

		    int payments = 0, t, n, result;
		    string data(recvbuf), _case, answer;

		    if (data[0] == '*') {
		        cout << data << endl;
		        answer = "enter";

		    } else if (data[0] == 'h' || data[0] == 't' || data[0] == 'w') {
		        cout << data << endl;
		        exit( 0 );
		    }

		    else if (data[0] == 'C') {
		        stringstream ssdata;
		        ssdata << data;

		        ssdata >> _case;   cout << _case << " ";
		        ssdata >> _case;   cout << _case << endl;
		        ssdata >> t >> n;  cout << t << " " << n << endl;

		        while(ssdata >> x[payments++]);

		        while (payments < n - 1) {
		            memset( recvbuf, 0, DEFAULT_BUFLEN );
		            recvResult = recv( ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0 );
		            cout << "data part recvbuf[" << recvResult << "]" << endl;
		            string part(recvbuf);
		            data += part;

		            stringstream sspart;
		            sspart << part;

		            while (sspart >> x[payments++]);
		        }

		        result = get_result(t, n);

		        answer = to_string(result);

		    }

            answer += "\n";
		    sendbuf = (char*)answer.c_str();

		    printf( "sendbuf[%d]: %s\n", (int)strlen(sendbuf), sendbuf );

            sendResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
            if( sendResult == SOCKET_ERROR ) {
                printf( "Send failed\n" );
                close( ConnectSocket );
                exit( -5 );
            }

        } else if( recvResult == 0 )
            cout << "Connection closed\n";

        else {
            printf( "Recv failed\n" );
            close( ConnectSocket );
            exit( -6 );
        }

    } while( recvResult > 0 );
}

int get_result(int t, int n) {
    int imin, imax, vmin, vmax, diff;
    imin = imax = diff = 0;

    s.clear();

    for (int i = 0; i < n; i++) {

        if (i > 0) {
            it = s.begin();
            vmin = *(it);
            it = s.end();
            vmax = *(--it);

            if (abs(vmin - x[i]) <= t && abs(vmax - x[i]) <= t) {
                s.insert(x[i]);
                imax = i;

            } else {
                diff = max(diff, imax - imin + 1);

                s.insert(x[i]);
                imax = i;

                it = s.begin();
                vmin = *(it);
                it = s.end();
                vmax = *(--it);

                while (imin < i && (abs(vmin - x[i]) > t || abs(vmax - x[i]) > t)) {
                    s.erase( s.find(x[imin]) );

                    it = s.begin();
                    vmin = *(it);
                    it = s.end();
                    vmax = *(--it);

                    imin++;
                }
            }

        } else {
            s.insert(x[i]);
            imin = imax = i;
            diff = 1;
        }
    }
    diff = max(diff, imax - imin + 1);

    return diff;
}

int main( int argc, char *argv[] ) {
    printf( "argc: %d\n", argc );
    for( int i = 0; i < argc; i++ )
        printf( "argv[%d]: %s\n", i, argv[i] );
    putchar( '\n' );

    if( argc == 3 ) {
        Client client( argv[1], atoi(argv[2]) );
        printf( "%s client[%s]\n", client.hostName, client.getLocalIP() );

        client.socketClient();
        cout << "aguardando Servidor ..." << endl;

        client.connectServer();
        printf( "Server %s Online\n", client.getServerIP() );

        client.send_recv();
    }

    return 0;
}
