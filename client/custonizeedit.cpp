#include "custonizeedit.h"

CustonizeEdit::CustonizeEdit(QWidget *parent):QLineEdit (parent),_max_len(0)
{
    connect(this, &QLineEdit::textChanged, this, &CustonizeEdit::limitTextLength);
}

void CustonizeEdit::SetMaxLength(int maxLen)
{
    _max_len = maxLen;
}
