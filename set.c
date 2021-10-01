#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

//struktura opisująca kartę
struct karta {
    char a; //liczba figur
    char b; //kolor figur
    char c; //wypełnienie figur
    char d; //kształt figur
};
typedef struct karta karta;

//maksymalna liczba kart na stole - rozmiar tablicy kart
#define N 81

//wypelnia tablicę kart zerami
void zeruj(karta tab[])
{
    for (int i = 0; i < N; i++)
    {
        tab[i].a = tab[i].b = tab[i].c = tab[i].d = '0';
    }
}

//wypelnia tablicę tab w indeksie i, w polu wyznaczonym przez zmienną licz
void wypelnij_tablice(karta tab[], char x, int i, int licz)
{
    if (licz == 1)
        tab[i].a = x;
    else if (licz == 2)
        tab[i].b = x;
    else if (licz == 3)
        tab[i].c = x;
    else
        tab[i].d = x;
}

//wczytuje dane do tablicy tab i nadaje parametrowi ile_wszystkich wartość
//taką, ile jest kart jest w grze
void wczytaj(karta tab[], int *ile_wszystkich)
{
    char x;
    int i = 0;
    int licz = 0;
    while (scanf("%c", &x) != EOF)
    {
        if (isdigit(x))
        {
            licz++;
            wypelnij_tablice(tab, x, i, licz);
            if (licz == 4)
            {
                i++;
                licz = 0;
            }
        }
    }
    *ile_wszystkich = i;
}

//przekazuje minimum z dwoch liczb a i b
int mini(int a, int b)
{
    if (a <= b)
        return a;
    return b;
}

//porównuje trzy wartości - jesli są wszystkie takie same lub parami rozne
// to przekazuje 1, w przeciwnym przypadku przekazuje 0
bool porownaj_elementy(int x, int y, int z)
{
    if ((x == y && x == z) || (x != y && y != z && x != z))
    {
        return 1;
    }
    return 0;
}

//porównuje trzy karty - jesli tworzą SET to przekazuje 1,
// w przeciwnym przypadku przekazuje 0
bool porownaj(karta tab[], int i, int j, int k)
{
    if (porownaj_elementy(tab[i].a, tab[j].a, tab[k].a) == 1)
    {
        if (porownaj_elementy(tab[i].b, tab[j].b, tab[k].b) == 1)
        {
            if (porownaj_elementy(tab[i].c, tab[j].c, tab[k].c) == 1)
            {
                if (porownaj_elementy(tab[i].d, tab[j].d, tab[k].d) == 1)
                    return 1;
            }
        }
    }
    return 0;
}

//sprawdza czy pod indeksem i jest karta, 
//jesli tak, to przekazuje 1, jesli nie to przekazuje 0
bool niepusta(karta tab[], int i)
{
    if (tab[i].a != 0)
        return true;
    return false;
}

//sprawdza, czy pod indeksami i,j,k są karty (czy nie ma samych zer)
bool niepuste_trzy(karta tab[], int i, int j, int k)
{
    if (niepusta(tab, i) && niepusta(tab, j) && niepusta(tab, k))
        return true;
    return false;
}

//sprawdza czy  indeksy i,j,k są różne
bool rozne_indeksy(int i, int j, int k)
{
    if (i != j && i != k && j != k)
        return true;
    return false;
}

//sprawdza, czy na stole jest SET
bool sprawdz(karta tab[], int ile_uzytych, int *x, int *y, int *z)
{
    bool czy_set = 0;
    for (int i = 0; i < ile_uzytych && !czy_set; i++)
    {
        for (int j = 1; j < ile_uzytych && !czy_set; j++)
        {
            for (int k = 2; k < ile_uzytych && !czy_set; k++)
            {
                //jeśli na stole jest SET to zapamiętujemy indeksy kart w jego składzie
                if (niepuste_trzy(tab, i, j, k) && rozne_indeksy(i, j, k) && porownaj(tab, i, j, k) == 1)
                {
                    *x = i;
                    *y = j;
                    *z = k;
                    czy_set = 1;
                }
            }
        }
    }
    return czy_set;
}

//wypisuje jedną kartę za tablicy tab o indeksie i
void wypisz_karte(karta tab[], int i)
{
    printf(" %c%c%c%c", tab[i].a, tab[i].b, tab[i].c, tab[i].d);
}

// wypisuje trzy karty o indeksach x, y, z z tablicy tab
void wypisz_trzy(karta tab[], int x, int y, int z)
{
    wypisz_karte(tab, x);
    wypisz_karte(tab, y);
    wypisz_karte(tab, z);
    printf("\n");
}

//zeruje tablicę tab w indeksie i
//(na miejscu karty, która została zdjęta ze stołu)
void zeruj_karte(karta tab[], int i)
{
    tab[i].a = tab[i].b = tab[i].c = tab[i].d = 0;
}

// zeruje tablicę tab w indekksach x, y, z, tam, gdzie były karty
// tworzące SET
void zeruj_trzy(karta tab[], int x, int y, int z)
{
    zeruj_karte(tab, x);
    zeruj_karte(tab, y);
    zeruj_karte(tab, z);
}

//wypisuje karty, które tworzyły SET i usuwa je z tablicy
//(zeruje tablicę w indeksach x, y, z)
void odejmij_trzy(karta tab[], int x, int y, int z, int *ile_uzytych, int *ile_zostalo, int *ile_na_stole)
{
    printf("-");
    wypisz_trzy(tab, x, y, z);
    zeruj_trzy(tab, x, y, z);
    *ile_na_stole -= 3;
    if(*ile_zostalo>0 && *ile_na_stole < 12)
    {
        *ile_uzytych += 3;
        *ile_na_stole += 3;
        *ile_zostalo -= 3;
    }
}

//wypisuje "+", zmienia wartości parametrów
void dobierz_trzy(int *ile_uzytych, int *ile_zostalo, int *ile_na_stole)
{
    printf("+\n");
    *ile_uzytych += 3;
    *ile_zostalo -= 3;
    *ile_na_stole += 3;
}

//wypisuje karty, które są teraz na stole
void wypisz_stol(karta tab[], int ile_uzytych)
{
    printf("=");
    for (int i = 0; i < ile_uzytych; i++)
    {
        if (niepusta(tab, i))
        {
            wypisz_karte(tab, i);
        }
    }
    printf("\n");
}

//symuluje rozgrywkę
void graj(karta tab[], int ile_wszystkich)
{
    //ile kart jest teraz na stole
    int ile_na_stole = mini(12, ile_wszystkich);
    //ile kart zostalo wykorzystanych w trakcie gry
    int ile_uzytych = ile_na_stole;
    //ile kart jest jeszcze do wykorzystania poza stołem
    int ile_zostalo = ile_wszystkich - ile_uzytych;
    //x, y, z zapamiętują indeksy kart, które tworzą SET
    int x;
    int y;
    int z;
    //jesli zostal wykonany ruch (lub jesli to poczatek gry),
    // to zmienna byla_zmiana ma wartość 1
    int byla_zmiana = 1;
    wypisz_stol(tab, ile_uzytych);
    while (ile_zostalo != 0 || byla_zmiana == 1)
    {
        byla_zmiana = 0;
        //jesli na stole jest SET
        if (sprawdz(tab, ile_uzytych, &x, &y, &z) == 1)
        {
            byla_zmiana = 1;
            odejmij_trzy(tab, x, y, z, &ile_uzytych, &ile_zostalo, &ile_na_stole);
            wypisz_stol(tab, ile_uzytych);
        }
        //na stole nie ma SET, ale zostaly jeszcze karty w talii
        else if (ile_zostalo != 0)
        {
            dobierz_trzy(&ile_uzytych, &ile_zostalo, &ile_na_stole);
            byla_zmiana = 1;
            wypisz_stol(tab, ile_uzytych);
        }
    }
}

int main() {

    karta wszystkie[N];
    zeruj(wszystkie);
    int ile_wszystkich;
    wczytaj(wszystkie, &ile_wszystkich);
    graj(wszystkie, ile_wszystkich);
    return 0;
}
