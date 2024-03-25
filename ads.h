#ifndef ADS_H
#define ADS_H

#include "html.h"

struct Metro
{
    QString metro;
    int doMetroPeshkomMinut;
};

struct Ad
{
    QString url;
    QString html;
    QString caption;
    QString adress;
    std::vector<Metro> metros;
};

#endif // DSFHB_H
