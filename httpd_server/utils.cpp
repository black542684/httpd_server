#include "utils.h"
/*
* ��ȡ�����һ��
*/
int get_line(SOCKET sock, char* buff, int size) {

	char c = '\0'; // �����ȡ����һ���ַ�
	int i = 0; // �±�-  buff[i]


	while (i < size - 1 && c != '\n')
	{
		// �ӻ������ж�ȡһ���ַ�
		int n = recv(sock, &c, 1, 0);

		// recv����������ʵ��copy���ֽ���������0��ʾ��ȡ�����ַ�
		if (n > 0) {
			// ���ڻ��з���\r\n ���ָ�ʽ����Ҫ�ж�
			if (c == '\r') {
				// �鿴��һ���ַ��ǲ���\n
				n = recv(sock, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n') {
					recv(sock, &c, 1, 0); // ������ȡ
				}
				else {
					c = '\n';
				}
			}
			buff[i++] = c;
		}
		else {
			// ��ȡʧ�ܵ����
			int id = WSAGetLastError();
			switch (id)
			{
			case WSANOTINITIALISED: printf("��δִ�гɹ��� WSAStartup\n"); break;
			case WSASYSNOTREADY: printf("������ϵͳ�����á�\n"); break;
			case WSAHOST_NOT_FOUND: printf("�Ҳ���������\n"); break;
			case WSATRY_AGAIN: printf("�Ҳ�������Ȩ������\n"); break;
			case WSANO_RECOVERY: printf("���ǲ��ɻָ��Ĵ���\n"); break;
			case WSAEINPROGRESS: printf("�������ڽ����С�\n"); break;
			case WSANO_DATA: printf("��Ч���ƣ�û���������͵����ݼ�¼��\n"); break;
			case WSAEINTR: printf("�жϵĺ������á�\n"); break;
			case WSAEPROCLIM: printf("���̹��ࡣ\n");
			case WSAEFAULT: printf("��ַ����\n");
			case WSAENOTSOCK: printf("��Ч�׽����ϵ��׽��ֲ������ṩ���׽��־����Ч��\n");
			default: printf("δ֪���� id = %d\n", id); break;
			};
			printf("�յ�����.\n");
			c = '\n';
		}
	}

	buff[i] = '\0'; // ������
	return i;
}

/*
* �����ո�
*/
void trimStart(char* str, int size, int* index) {
	while (*index < size)
	{
		if (isspace(str[*index])) {
			*index += 1;
		}
		else {
			break;
		}
	}
}