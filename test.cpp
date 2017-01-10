void *communication(char *device, tBuff Recv[2]) {
    tBuff oRecv = Recv[0];
    tBuff iRecv = Recv[1];
    cleanBuff(&oRecv);
    cleanBuff(&iRecv);

    while (!quit) {
        pthread_mutex_unlock(&mtx);
        usleep(500);
        pthread_mutex_lock(&mtx);
        int n =
                read(hSerial, &(oRecv.chArrBuff[oRecv.iRecvLength]), sizeof (char));

        if (n != 0 && n != -1 && oRecv.chArrBuff[oRecv.iRecvLength] != 0) {
            oRecv.iRecvLength++;
            if (oRecv.iRecvLength > 3)
                if (oRecv.chArrBuff[oRecv.iRecvLength - 2] == 13) {
                    oRecv.chArrBuff[oRecv.iRecvLength - 1] = '\0';
                    oRecv.chArrBuff[oRecv.iRecvLength - 2] = '\0';
                    char cd[72];
                    memset(cd, '\0', 72);
                    sprintf(cd, "IN: %s", oRecv.chArrBuff);
                    output_stdout(cd);
                    cleanBuff(&oRecv);
                }
            if (oRecv.iRecvLength == (BUFFLEN - 1)) {
                cleanBuff(&oRecv);
            }
        }
        if (iRecv.iRecvLength) {
            char cd[36];
            sprintf(cd, "%s\r\n", iRecv.chArrBuff);
            write(hSerial, cd, sizeof (char) * (iRecv.iRecvLength + 3));
            sprintf(cd, "OUT: %s (%i)", iRecv.chArrBuff, iRecv.iRecvLength);
            output_stdout(cd);
            iRecv.iRecvLength = 0;
        }
    }
}