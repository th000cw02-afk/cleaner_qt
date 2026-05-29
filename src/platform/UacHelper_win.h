#ifndef UACHELPER_WIN_H
#define UACHELPER_WIN_H

class UacHelper {
public:
    static bool restartElevated(const wchar_t* arguments = nullptr);
};

#endif // UACHELPER_WIN_H
