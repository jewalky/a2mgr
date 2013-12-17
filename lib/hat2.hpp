#pragma once

int send_msg(SOCKET sock, int ver, u_char *msg, size_t len, int timeout);
