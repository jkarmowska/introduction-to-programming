#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define liczba_znakow 26//liczba dostepnych zmiennych

//struktura opisująca liczbę
struct liczba {
    int *cyfry; //tablica przechowująca wartość liczby
    int dlugosc; //długość liczby w systemie o podstawie 10000
    int rozmiar; //rozmiar tablicy cyfry
};
typedef struct liczba liczba;

//struktura opisująca polecenie
struct polecenie {
    char instrukcja; //napis oznaczający polecenie
    int zmiennna1; //parametr 1 polecenia
    int zmiennna2; //parametr 2 polecenia
    int adres; //parametr 3 polecenia
};
typedef struct polecenie polecenie;


//zwraca  minimum z 2*n+1 i INT_MAX
int wiecej(int n)
{
    if (n > (INT_MAX - 1) / 2)
        return INT_MAX;
    return 1 + 2 * n;
}

//realokuje pamiec tablicy polecenia
void alokuj_polecenia(polecenie **polecenia, int *rozmiar)
{
    *rozmiar = wiecej(*rozmiar);
    *polecenia = (polecenie *) realloc(*polecenia, (size_t) (*rozmiar) * sizeof(polecenie));
}

//tworzy tablicę zmienne[liczba_znakow] struktur liczba
void inicjuj_zmienne(liczba *zmienne)
{
    for (int i = 0; i < liczba_znakow; i++)
    {
        zmienne[i].dlugosc = 1;
        zmienne[i].cyfry = (int *) calloc(1, sizeof(int));
        zmienne[i].rozmiar = 1;
    }
}

//realokuje pamięć tablicy zmienne
void realokuj_zmienne(liczba *zmienne, int litera)
{
    zmienne[litera].rozmiar = wiecej(zmienne[litera].rozmiar);
    zmienne[litera].cyfry = (int *) realloc(zmienne[litera].cyfry,
                                            (size_t) (zmienne[litera].rozmiar) * sizeof(zmienne[litera].cyfry[0]));
}

//zwalnia pamięć tablicy zmienne
void zwolnij_zmienne(liczba *zmienne)
{
    for (int i = 0; i < liczba_znakow; i++)
    {
        free(zmienne[i].cyfry);
    }
}

//sprawdza, czy tablica polecenia jest wystarczająco duza, jesli nie, to wywołuje funkcję zwiększającą rozmiar
void powiekszanie(polecenie **polecenia, int *rozmiar, int liczba_polecen)
{
    while (liczba_polecen >= *rozmiar)
    {
        alokuj_polecenia(polecenia, rozmiar);
    }
}

//zapisuje w tablicy polecenia instrukcję JMP pod indeksem ostatni
// i dopisuje adres do instrukcji DJZ pod indeksem pierwszy
void zapisz_JMP(polecenie *polecenia, int pierwszy, int ostatni)
{
    polecenia[ostatni].instrukcja = 'J';
    polecenia[ostatni].adres = pierwszy;
    polecenia[pierwszy].adres = ostatni + 1;
}

//zapisuje w tablicy polecenia instrukcję ADD pod indeksem ostatni
void zapisz_ADD(polecenie *polecenia, int indeks, int zmienna)
{
    polecenia[indeks].instrukcja = 'A';
    polecenia[indeks].zmiennna1 = polecenia[indeks + 1].zmiennna1;
    polecenia[indeks].zmiennna2 = zmienna;
}

//zapisuje w tablicy polecenia instrukcję HLT pod indeksem ostatni
void zapisz_HLT(polecenie *polecenia, int indeks)
{
    polecenia[indeks].instrukcja = 'H';
}

//zapisuje w tablicy polecenia instrukcję ktore pod indeksem ostatni
void zapisz_CLR_INC_DJZ(polecenie *polecenia, int indeks, int zmienna, char ktore)
{
    polecenia[indeks].instrukcja = ktore;
    polecenia[indeks].zmiennna1 = zmienna;

}

//optymalizuje kod maszynowy miedzy indeksami poczatek i koniec
void optymalizuj(polecenie *polecenia, int poczatek, int koniec)
{
    int indeks = poczatek;
    int zmienna = polecenia[poczatek].zmiennna1;
    while (indeks < koniec - 1)
    {
        zapisz_ADD(polecenia, indeks, zmienna);
        indeks++;
    }
    zapisz_CLR_INC_DJZ(polecenia, indeks, zmienna, 'C');
}

//funkcja rekurencyjna, modyfikuje kod maszynowy pętli, zwraca 0 jesli kod można zoptymalizowac
bool powtarzaj(polecenie **polecenia, int *rozmiar, int poczatek, int *liczba_polecen)
{
    bool wynik = 0;
    int znak = getchar();
    powiekszanie(polecenia, rozmiar, *liczba_polecen);
    zapisz_CLR_INC_DJZ(*polecenia, *liczba_polecen, znak, 'D');
    (*liczba_polecen)++;
    znak = getchar();
    while (znak != ')')
    {
        if (znak == (*polecenia)[poczatek].zmiennna1)
            wynik = 1;
        if (znak == '(')
        {
            wynik = 1;
            int pocz = *liczba_polecen;
            if (powtarzaj(polecenia, rozmiar, pocz, liczba_polecen))
            {
                powiekszanie(polecenia, rozmiar, *liczba_polecen);
                zapisz_JMP(*polecenia, pocz, *liczba_polecen);
                (*liczba_polecen)++;
            } else
                optymalizuj(*polecenia, pocz, *liczba_polecen);
        } else//znak=a,...,z
        {
            powiekszanie(polecenia, rozmiar, *liczba_polecen);
            zapisz_CLR_INC_DJZ(*polecenia, *liczba_polecen, znak, 'I');
            (*liczba_polecen)++;
        }
        znak = getchar();
    }
    return wynik;
}


//wywoluje funkcję rekurencyjną po raz pierwszy
void wywolaj_powtarzanie(polecenie **polecenia, int *rozmiar, int poczatek, int *liczba_polecen)
{
    if (powtarzaj(polecenia, rozmiar, poczatek, liczba_polecen))
    {
        powiekszanie(polecenia, rozmiar, *liczba_polecen);
        zapisz_JMP(*polecenia, poczatek, *liczba_polecen);
        (*liczba_polecen)++;
    } else
    {
        optymalizuj(*polecenia, poczatek, *liczba_polecen);
    }
}

//wypisuje na ekranie liczbę pod indeksem a w tablicy zmienne
void wypisz_liczbe(liczba *zmienne, int a)
{
    int ile = zmienne[a].dlugosc;
    for (int i = ile - 1; i >= 0; i--)
    {
        if (i == ile - 1)
        {
            printf("%d", zmienne[a].cyfry[i]);
        } else printf("%04d", zmienne[a].cyfry[i]);
    }
    printf("\n");
}

//wczytuje i zapisuje w tablicy polecenia kod maszynowy
void kod_maszynowy(polecenie **polecenia)
{
    int znak = getchar();
    int rozmiar = 0;//rozmiar tablicy polecen
    int liczba_polecen = 0;//ile polecen jest zapisanych w tablicy
    while (znak != EOF && znak != '\n')
    {
        if (znak == '(')
        {
            int poczatek = liczba_polecen;
            wywolaj_powtarzanie(polecenia, &rozmiar, poczatek, &liczba_polecen);
        } else//znak=a,...,z
        {
            powiekszanie(polecenia, &rozmiar, liczba_polecen);
            zapisz_CLR_INC_DJZ(*polecenia, liczba_polecen, znak, 'I');
            liczba_polecen++;
        }
        znak = getchar();
    }
    powiekszanie(polecenia, &rozmiar, liczba_polecen);
    zapisz_HLT(*polecenia, liczba_polecen);
    liczba_polecen++;
    if (znak == EOF)
        ungetc(znak, stdin);
}

//jesli to konieczne, wywoluje funkcję realokująca tablicę wartość pod indeksem litera w tablicy zmienne
void powiekszanie_zmiennych(liczba *zmienne, int litera)
{
    while (zmienne[litera].dlugosc >= zmienne[litera].rozmiar)
    {
        realokuj_zmienne(zmienne, litera);
    }
}

//dodaje jeden do wartości zmiennej pod indeksem litera w tablicy zmienne
void dodaj_jeden(liczba *zmienne, int litera)
{
    int przeniesienie;
    int i = 1;
    (zmienne[litera].cyfry[0])++;
    przeniesienie = zmienne[litera].cyfry[0] / 10000;
    zmienne[litera].cyfry[0] %= 10000;
    while (i < zmienne[litera].dlugosc && przeniesienie)
    {
        zmienne[litera].cyfry[i]++;
        przeniesienie = zmienne[litera].cyfry[i] / 10000;
        zmienne[litera].cyfry[i] %= 10000;
        i++;
    }
    if (przeniesienie)
    {
        (zmienne[litera].dlugosc)++;
        powiekszanie_zmiennych(zmienne, litera);
        zmienne[litera].cyfry[(zmienne[litera].dlugosc) - 1] = 1;
    }

}

//zeruje wartość zmiennej pod indeksem a w tablicy zmienne
void zeruj(liczba *zmienne, int a)
{
    zmienne[a].dlugosc = 1;
    zmienne[a].rozmiar = 1;
    zmienne[a].cyfry = (int *) realloc(zmienne[a].cyfry, sizeof(int));
    zmienne[a].cyfry[0] = 0;
}

//odejmuje jeden id zmiennej pod indeksem a w tablicy zmienne
void odejmij(liczba *zmienne, int a)
{
    int pozycz = 1;
    int i = 0;
    int ostatni = zmienne[a].dlugosc - 1;
    //
    while (i < ostatni && pozycz)
    {
        if (zmienne[a].cyfry[i] == 0)
        {
            pozycz = 1;
            zmienne[a].cyfry[i] = 9999;
        } else
        {
            zmienne[a].cyfry[i]--;
            pozycz = 0;
        }
        i++;
    }
    if (pozycz && zmienne[a].cyfry[ostatni] == 1)
    {
        zmienne[a].cyfry[ostatni] = 0;
        if (ostatni != 0)
            zmienne[a].dlugosc -= pozycz;
    } else if (pozycz && zmienne[a].cyfry[ostatni] > 1)
    {
        zmienne[a].cyfry[ostatni] -= pozycz;
    }
}

//dodaje wartość zmiennej pod indeksem litera2 do zmiennej pod indeksem litera1 w tablicy zmienne
void dodaj_zmienna(liczba *zmienne, int litera1, int litera2)
{
    int przeniesienie = 0;
    int i = 0;
    int nowa;
    int dlugosc1 = zmienne[litera1].dlugosc;
    int dlugosc2 = zmienne[litera2].dlugosc;
    while (i < dlugosc1 && i < dlugosc2)
    {
        nowa = zmienne[litera1].cyfry[i] + zmienne[litera2].cyfry[i] + przeniesienie;
        przeniesienie = nowa / 10000;
        zmienne[litera1].cyfry[i] = nowa % 10000;
        i++;
    }
    if (dlugosc1 > dlugosc2)
    {
        while (i < dlugosc1)
        {
            nowa = zmienne[litera1].cyfry[i] + przeniesienie;
            przeniesienie = nowa / 10000;
            zmienne[litera1].cyfry[i] = nowa % 10000;
            i++;
        }
    } else if (dlugosc1 < dlugosc2)
    {
        while (i < dlugosc2)
        {
            zmienne[litera1].dlugosc++;
            powiekszanie_zmiennych(zmienne, litera1);
            nowa = zmienne[litera2].cyfry[i] + przeniesienie;
            przeniesienie = nowa / 10000;
            zmienne[litera1].cyfry[i] = nowa % 10000;
            i++;
        }
    }
    if (przeniesienie)
    {
        zmienne[litera1].dlugosc++;
        powiekszanie_zmiennych(zmienne, litera1);
        zmienne[litera1].cyfry[i] = przeniesienie;
    }
}

//interpretuje kod maszynowy w tablicy polecenia i odpowiednio modyfikuje wartości w tablicy zmienne
void interpreter(polecenie *polecenia, liczba *zmienne)
{
    int i = 0;
    int litera;
    int licz = 0;
    while (polecenia[i].instrukcja != 'H')
    {
        switch (polecenia[i].instrukcja)
        {
            case 'C':
                zeruj(zmienne, polecenia[i].zmiennna1 - 'a');
                i++;
                break;
            case 'I':
                dodaj_jeden(zmienne, polecenia[i].zmiennna1 - 'a');
                i++;
                break;
            case 'D':
                licz++;
                litera = polecenia[i].zmiennna1 - 'a';
                if (zmienne[litera].dlugosc == 1 && zmienne[litera].cyfry[0] == 0)
                {
                    i = polecenia[i].adres;
                } else
                {
                    litera = polecenia[i].zmiennna1 - 'a';
                    odejmij(zmienne, litera);
                    i++;
                }
                break;
            case 'J':
                i = polecenia[i].adres;
                break;
            case 'A':
                dodaj_zmienna(zmienne, polecenia[i].zmiennna1 - 'a', polecenia[i].zmiennna2 - 'a');
                i++;
                break;
        }
    }
}

//tworzy tablicę zmiennych, wywoluje funkcję tworzącą kod maszynowy i funkcję interpretującą
//lubfunkcję, która wypisuje na ekranie wartość zmiennej
void program()
{
    int znak = getchar();
    liczba zmienne[liczba_znakow];
    inicjuj_zmienne(zmienne);
    polecenie *polecenia;
    while (znak != EOF)
    {
        if (znak == '=')
        {
            znak = getchar();
            wypisz_liczbe(zmienne, znak - 'a');
            znak = getchar();
            if (znak == EOF)
                ungetc(znak, stdin);
        } else //znak=a,...,z,(,)
        {
            polecenia = NULL;
            ungetc(znak, stdin);
            kod_maszynowy(&polecenia);
            interpreter(polecenia, zmienne);
            free(polecenia);
        }
        znak = getchar();
    }
    zwolnij_zmienne(zmienne);
}

int main(void)
{
    program();
}