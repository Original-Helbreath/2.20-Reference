/*
 * Helbreath.exe (retail 2.02, PE timestamp 2001-08-21 07:18:17 UTC) - "Helbreath Auto-Update".
 * Internal name "mfcftp"; MFC 4.2 (VC6, statically linked) dialog app + zlib 1.0.4.
 * Cleaned from Ghidra pseudo-C (program Helbreath_202.exe) and capstone disassembly where the
 * decompiler lost the frame (functions with a large __chkstk frame: 0x401bd0, 0x4023e0, 0x402510).
 * Names are ours. Addresses are virtual (image base 0x400000).
 * MFC/CRT/zlib helpers are called by their recognised names; see ../README.md for the skip list.
 */

#define ERR_TOO_BIG      (-1001)   /* 0xfffffc17 */
#define ERR_LIST         (-1002)   /* 0xfffffc16 */
#define ERR_DOWNLOAD     (-1003)   /* 0xfffffc15 */
#define ERR_EXTRACT      (-1004)   /* 0xfffffc14 */

/* One update.lst record: 0xe4 = 228 bytes (read with _read(fd, rec, 0xe4)). */
typedef struct {
    char   szLocalPath[110];   /* +0x00  path of the game file, relative to the install dir      */
    char   szGzName[110];      /* +0x6e  name of the gzip file on the FTP server (and temp local) */
    time_t tModified;          /* +0xdc  expected last-write time of szLocalPath                  */
    int    iSize;              /* +0xe0  expected size of szLocalPath, also used for progress     */
} UPDATE_ENTRY;

static int g_bCancel;          /* 0x43b0f0 - read by IsCancelled(), never written: always 0 */

/* 0x4010b0  CMfcftpApp::InitInstance */
BOOL CMfcftpApp::InitInstance()
{
    if (!AfxSocketInit(NULL)) {                 /* 0x42abc9 */
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED); /* string 0x67: "Windows sockets initialization failed." */
        return FALSE;
    }
    AfxEnableControlContainer(NULL);            /* 0x408828 */
    Enable3dControls();                         /* 0x42798d */
    CMfcftpDlg dlg;                             /* 0x4011f0, IDD 0x66 */
    m_pMainWnd = &dlg;
    dlg.DoModal();                              /* 0x420aa5 */
    return FALSE;                               /* no SetRegistryKey, no profile use */
}

/* 0x4011f0  CMfcftpDlg::CMfcftpDlg (IDD 0x66, icon resource 0x80) */
/* members: +0x5c CProgressCtrl m_progress (HWND at +0x78), +0x98 CString m_strStatus,
 *          +0x9c CString m_strProgress, +0xa0 m_pAutoProxy, +0xa4 HICON m_hIcon */

/* 0x4013d0  CMfcftpDlg::OnInitDialog */
BOOL CMfcftpDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    /* wizard code: append "About mfcftp..." (string 0x65) to the system menu */
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);
    m_progress.SendMessage(PBM_SETRANGE, 0, MAKELPARAM(0, 100));   /* 0x401, 0x640000 */
    m_progress.SendMessage(PBM_SETSTEP, 2, 0);                     /* 0x404 */
    m_progress.SendMessage(PBM_SETPOS, 0, 0);                      /* 0x402 */
    m_strStatus = "";
    AfxBeginThread(UpdateThread, this);         /* 0x4016b0 -> 0x41f933 */
    return TRUE;
}

/* 0x401690 / 0x401650 / 0x401670  CanExit / OnClose / OnCancel (wizard automation-proxy code) */

/* 0x4016d0  worker thread: the whole update, then start the game */
UINT UpdateThread(LPVOID pParam)
{
    CMfcftpDlg *dlg = (CMfcftpDlg *)pParam;
    CString strCurDir, strErr, strArgs, strCaption;
    char szHost[100], szUser[100], szPass[100], szCmd[512], szExec[512], szCurDir[1024];
    HINTERNET hNet, hFtp = NULL;
    int iStart = 1;                             /* MessageBox result: IDOK(1) = start game */
    int r;

    GetCurrentDirectoryA(sizeof(szCurDir), szCurDir);
    strCurDir = szCurDir;                       /* kept but unused */
    dlg->UpdateData(FALSE);

    hNet = InternetOpenA("SIEMENTECH FTP ", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);   /* 0x402360 */
    strcpy(szHost, "update.helbreath.com");
    strcpy(szUser, "helsmt");
    strcpy(szPass, "helsmt");
    strCaption = "Helbeath Auto-Update";        /* sic */

    if (hNet == NULL) {
        dlg->UpdateData(TRUE);
        iStart = dlg->MessageBox("The update server is not working or there is network problem.\r\n\r\n"
                                 /* + Korean (CP949) "run the client?" */, strCaption, MB_OKCANCEL);
        goto launch;
    }
    /* 0x402380: InternetConnectA(hNet, host, port 0 = default 21, user, pass,
     *           INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0) */
    hFtp = InternetConnectA(hNet, szHost, 0, szUser, szPass, INTERNET_SERVICE_FTP, 0x08000000, 0);
    if (hFtp == NULL) {
        InternetCloseHandle(hNet);
        dlg->UpdateData(TRUE);
        iStart = dlg->MessageBox("An error occured while connecting.\r\nTry again later.\r\n\r\n"
                                 "Would you like to start game?", strCaption, MB_OKCANCEL);
        goto cleanup;
    }
    FtpSetCurrentDirectoryA(hFtp, "patch_en");                      /* 0x4023c0 */
    dlg->m_strStatus = "Download : update.lst_";
    dlg->UpdateData(FALSE);

    if (FtpDownload(hFtp, "update.lst_", "update.lst_") < 0) {       /* 0x4023e0 */
        InternetCloseHandle(hNet); InternetCloseHandle(hFtp);
        dlg->UpdateData(TRUE);
        iStart = dlg->MessageBox("Failed to download!!\r\n\r\nWould you like to start game?",
                                 strCaption, MB_OKCANCEL);
        goto cleanup;
    }
    GunzipFile("update.lst_", "update.lst");                        /* 0x4025c0, result ignored */

    r = ProcessUpdateList(hFtp, "update.lst", dlg);                 /* 0x401bd0 */
    if (r >= 0) goto ok;
    InternetCloseHandle(hNet); InternetCloseHandle(hFtp);
    switch (r) {
    case ERR_EXTRACT:  strErr = "An error occured while extracting files.\r\nPlease check you have "
                                "enough disk space.\r\n\r\nWould you like to start game?"; break;
    case ERR_DOWNLOAD: strErr = "Failed to download files.\r\nUpdate The server is not working or "
                                "network problem.\r\n\r\nWould you like to start game?"; break;
    case ERR_LIST:     strErr = "An error occured while making download list.\r\n Please contact to "
                                "game master.\r\n\r\nWould you like to start game?"; break;
    case ERR_TOO_BIG:  strErr = "You need to download too many files.\r\nPlease use homepage.\r\n\r\n"
                                "Would you like to start game?"; break;   /* built from 0x4362a0 */
    default:           strErr = "An error occured while updating.\r\nPlease contact to game master."
                                "\r\n\r\nWould you like to start game?";
    }
    dlg->UpdateData(TRUE);
    iStart = dlg->MessageBox(strErr, strCaption, MB_OKCANCEL);
ok:
cleanup:
    InternetCloseHandle(hNet);                  /* note: closes handles a second time on error paths */
launch:
    if (hFtp) InternetCloseHandle(hFtp);
    remove("update.lst_");                      /* 0x41eadc */
    remove("update.lst");

    /* Pass through whatever followed the launcher's own name on its command line. */
    memset(szCmd, 0, sizeof(szCmd)); memset(szExec, 0, sizeof(szExec));
    sprintf(szCmd, "%s", GetCommandLineA());
    char *p = strrchr(szCmd, '"');              /* quoted exe path: args follow the last quote */
    if (p == NULL) p = strchr(szCmd, ' ');      /* unquoted: args follow the first space */
    if (p != NULL) strArgs = p + 1;
    strArgs.TrimLeft(' '); strArgs.TrimRight(' ');
    if (strArgs.GetLength() > 0) sprintf(szExec, "%s %s", "HelGame.exe", (LPCSTR)strArgs);
    else                         sprintf(szExec, "%s", "HelGame.exe");

    dlg->UpdateData(FALSE);
    if (iStart == IDOK)
        WinExec(szExec, SW_SHOW);               /* relative name: resolved from the current dir / PATH */
    dlg->EndDialog(IDOK);                       /* 0x4024f0 */
    return 1;
}

/* 0x401bd0  read update.lst, download and unpack every out-of-date file */
int ProcessUpdateList(HINTERNET hFtp, const char *pszList, CMfcftpDlg *dlg)
{
    UPDATE_ENTRY rec;
    int fdList, fdTmp, fdOut, n, nFiles = 0, iFailed = 0;
    unsigned uTotal = 0, uDone;
    char szProgress[400], szStatus[256], szDir[200], buf[4096];

    if ((fdList = _open(pszList, _O_RDONLY | _O_BINARY)) < 0) return ERR_LIST;
    if ((fdTmp = _open("update.tmp", _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY, 0666)) < 0) {
        _close(fdList); return ERR_LIST;
    }
    /* pass 1: copy every record whose local file is missing or differs into update.tmp */
    for (;;) {
        memset(&rec, 0, sizeof(rec));
        if ((n = _read(fdList, &rec, 0xe4)) < 1) break;
        if (IsFileCurrent(&rec) == 0) continue;                     /* 0x402040 */
        if (_write(fdTmp, &rec, n) < 1) break;
        uTotal += rec.iSize;
        nFiles++;
    }
    _close(fdList); _close(fdTmp);
    if (nFiles == 0) return 0;                  /* already up to date */

    dlg->UpdateData(TRUE);
    if ((uTotal & 0xfff00000) > 0xc800000) {    /* more than ~200 MB to fetch */
        sprintf(szStatus, "file size = %d", uTotal);
        MessageBoxA(GetActiveWindow(), szStatus, "File Size", MB_OK);
        return ERR_TOO_BIG;
    }
    /* pass 2: fetch each listed .gz, then unpack it over the game file.
     * (Original reuses the update.tmp descriptor variables confusingly; behaviour shown.) */
    if ((fdTmp = _open("update.tmp", _O_RDONLY | _O_BINARY)) < 0) return ERR_LIST;
    uDone = 0;
    for (;;) {
        memset(&rec, 0, sizeof(rec));
        if (_read(fdTmp, &rec, 0xe4) < 1) break;
        _splitpath(rec.szGzName, NULL, szDir, NULL, NULL);          /* 0x402300 */
        _mkdir(szDir);
        sprintf(szProgress, "%dK download complete(Full %dM)", uDone / 1024, uTotal / (1024 * 1024));
        sprintf(szStatus, "Download : %s(%dK)", rec.szGzName, rec.iSize / 1024);
        SendMessageA(dlg->m_progress.m_hWnd, PBM_SETPOS, uDone * 100 / uTotal, 0);
        dlg->m_strProgress = szProgress;
        dlg->m_strStatus = szStatus;
        dlg->UpdateData(FALSE);

        HINTERNET hFile = FtpOpenFileA(hFtp, rec.szGzName, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);
        if (hFile == NULL ||
            (fdOut = _open(rec.szGzName, _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY, 0666)) < 0)
            return ERR_DOWNLOAD;
        for (;;) {
            DWORD dwRead = 0;
            if (g_bCancel == 1) { iFailed = -1; break; }
            BOOL ok = InternetReadFile(hFile, buf, sizeof(buf), &dwRead);
            if (ok == TRUE && dwRead == 0) break;                    /* EOF */
            SendMessageA(dlg->m_progress.m_hWnd, PBM_SETPOS, uDone * 100 / uTotal, 0);
            dlg->UpdateData(FALSE);
            _write(fdOut, buf, dwRead);         /* a failed read writes 0 bytes and retries forever */
        }
        _close(fdOut);
        InternetCloseHandle(hFile);
        if (iFailed == -1) break;
        uDone += rec.iSize;
        if (ExtractEntry(hFtp, &rec, dlg) < 0) {                    /* 0x402140 */
            _close(fdTmp);
            remove("update.tmp");
            return ERR_EXTRACT;
        }
    }
    _close(fdTmp);
    if (hFtp) InternetCloseHandle(hFtp);
    remove("update.tmp");
    return 0;
}

/* 0x402040  0 if the local file matches the record (same mtime to the second and same size), else -1 */
int IsFileCurrent(const UPDATE_ENTRY *rec)
{
    CFileStatus st;
    if (CFile::GetStatus(rec->szLocalPath, st)) {                  /* 0x4246a2 */
        CTime want(rec->tModified);
        if (st.m_mtime.GetYear()   == want.GetYear()   && st.m_mtime.GetMonth()  == want.GetMonth() &&
            st.m_mtime.GetDay()    == want.GetDay()    && st.m_mtime.GetHour()   == want.GetHour()  &&
            st.m_mtime.GetMinute() == want.GetMinute() && st.m_size == rec->iSize)
            return 0;   /* note: seconds are not compared (tm_sec is skipped) */
    }
    return -1;
}

/* 0x402140  gunzip rec->szGzName over rec->szLocalPath, stamp its time, delete the .gz */
int ExtractEntry(HINTERNET hUnused, const UPDATE_ENTRY *rec, CMfcftpDlg *dlg)
{
    char szStatus[256];
    int fdGz, fdOut, r;
    if ((fdGz = _open(rec->szGzName, _O_RDONLY | _O_BINARY)) < 0) return -1;
    sprintf(szStatus, "Extracting : %s", rec->szLocalPath);
    dlg->m_strStatus = szStatus;
    dlg->UpdateData(FALSE);
    if ((fdOut = OpenTargetFile(rec->szLocalPath)) < 0) return -1;  /* 0x402230 */
    r = GunzipFd(fdGz, fdOut);                                      /* 0x402620 */
    if (r >= 0) SetFdTime(fdOut, rec->tModified);                   /* 0x402330 */
    _close(fdGz); _close(fdOut);
    remove(rec->szGzName);
    return r;
}

/* 0x402230  make the directory, open the target for writing, stamp it 1980-01-01 first */
int OpenTargetFile(const char *path)
{
    char szDir[100];
    struct tm t = {0}; t.tm_year = 80; t.tm_mday = 1;              /* only the date is set */
    struct _utimbuf ut;
    _splitpath(path, NULL, szDir, NULL, NULL);
    _mkdir(szDir);
    int fd = _open(path, _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY, 0666);
    if (fd < 0) { _rmdir(szDir); return fd; }                      /* 0x41eb13 */
    ut.actime = mktime(&t); ut.modtime = mktime(&t);
    _futime(fd, &ut);                                               /* 0x4108af */
    return fd;
}

/* 0x402330 */
void SetFdTime(int fd, time_t t) { struct _utimbuf ut = { t, t }; _futime(fd, &ut); }

/* 0x4023e0  download one file from the current FTP directory */
int FtpDownload(HINTERNET hFtp, const char *pszRemote, const char *pszLocal)
{
    char buf[4096]; DWORD dwRead; int r = 0;
    int fd = _open(pszLocal, _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY, 0666);
    if (fd < 0) return -1;
    HINTERNET h = FtpOpenFileA(hFtp, pszRemote, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);
    if (h == NULL) return -1;
    for (;;) {
        dwRead = 0;
        if (g_bCancel == 1) { r = -1; break; }
        if (InternetReadFile(h, buf, sizeof(buf), &dwRead) == TRUE && dwRead == 0) break;
        _write(fd, buf, dwRead);
    }
    _close(fd);
    InternetCloseHandle(h);
    return r;
}

/* 0x4025c0  gunzip a named file to another (fopen "wb" + gzopen "rb") */
int GunzipFile(const char *gz, const char *out)
{
    FILE *f = fopen(out, "wb");                 /* modes at 0x4364e4 / 0x4364e8 */
    if (!f) return -1;
    gzFile g = gzopen(gz, "rb");                /* 0x402690 */
    if (!g) return -1;
    int r = GzCopy(g, f);                       /* 0x402510 */
    fclose(f); gzclose(g);
    return r;
}

/* 0x402620  same, on descriptors: gzdopen(dup(fdGz)), fdopen(dup(fdOut)) */
/* 0x402510  GzCopy: gzread 0x2000-byte chunks and fwrite them; -1 on short write, 0 otherwise
 *           (a gzread error ends the copy with 0, i.e. it is not reported). */
