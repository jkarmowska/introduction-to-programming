#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//stale oznaczające poszczegolne symbole zgodnie z kodami ASCII
const int Rockford = (int) '@';
const int puste = (int) ' ';
const int ziemia = (int) '+';
const int kamien = (int) 'O';
const int diament = (int) '$';
const int wyjscie = (int) 'X';
const int W = (int) 'w';
const int S = (int) 's';
const int A = (int) 'a';

//struktura opisująca pole na planszy
struct pole {
    int x; //numer wiersza
    int y; //numer kolumny
};
typedef struct pole pole;

//wczytuje planszę do tablicy t, w zmiennej aktualne zapamięteuje pole, na którym jest Rockford
// oraz w zmiennej zostalo zapamietuje ile diamentow jest na planszy
void wczytaj_plansze(int w, int k, int **t, pole *aktualne, int *zostalo, bool *koniec)
{
    int symbol;
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < k; j++)
        {
            symbol = getchar();
            t[i][j] = symbol;
            if (symbol == Rockford)
            {
                aktualne->x = i;
                aktualne->y = j;
                *koniec = 0;
            } else if (symbol == diament)
                *zostalo += 1;
        }
        getchar();
    }
}

//wypisuje tablicę dwuwymiarową t, w wierszy, k kolumn
void wypisz_plansze(int w, int k, int **t)
{
    printf("%d %d\n", w, k);
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < k; j++)
            putchar(t[i][j]);
        printf("\n");
    }
}

//przesuwa kamien lub diament o jedno pole w dół na planszy t
//a= diament lub a=kamien
void przesun_symbol(int i, int j, int **t, int a)
{
    t[i][j] = puste;
    t[i + 1][j] = a;
}

//sprowadza planszę do stanu stabilnego
void stabilny(int w, int k, int **t)
{
    bool zmiana = 1;//czy na planszy zostala wykonana modyfikacja
    int a;
    while (zmiana)
    {
        zmiana = 0;
        for (int i = 1; i < w - 1; i++)
        {
            for (int j = 1; j < k - 1; j++)
            {
                a = t[i][j];
                if ((a == diament || a == kamien) && t[i + 1][j] == puste)
                {
                    przesun_symbol(i, j, t, a);
                    zmiana = 1;
                }
            }
        }
    }
}

//wykonuje ruch Rockforda w górę lub w dół (jeśli to możliwe)
//kierunek=-1 w górę, kierunek=1 w dół
//zostalo - ile diamentow jest jeszcze na planszy
//aktualne - pole, na którym jest Rockford
void przesun_pionowo(int **plansza, pole *aktualne, int *zostalo, int kierunek, bool *koniec)
{
    if (!*koniec)//jesli Rockford jest na planszy
    {
        pole cel;
        cel.x = aktualne->x + kierunek;
        cel.y = aktualne->y;
        int pom = plansza[aktualne->x + kierunek][aktualne->y]; //pom - symbol na polu docelowym
        if (pom == puste || pom == ziemia || pom == diament)//jesli jest mozliwy ruch na pole docelowe
        {
            if (pom == diament)
                *zostalo -= 1;
            plansza[aktualne->x + kierunek][aktualne->y] = Rockford;
            plansza[aktualne->x][aktualne->y] = puste;
            aktualne->x = cel.x;
        }
        if (pom == wyjscie && *zostalo == 0)
        {
            plansza[aktualne->x][aktualne->y] = puste;
            *koniec = 1;
        }
    }
}

//wykonuje ruch Rockforda w prawo lub w lewo (jeśli to możliwe)
//kierunek=-1 w lewo, kierunek=1 w prawo
//zostalo - ile diamentow jest jeszcze na planszy
//aktualne - pole, na którym jest Rockford
//gdy koniec=0 Rockford jest na planszy, gdy koniec=1 Rockforda nie ma na planszy
void przesun_poziomo(int **plansza, pole *aktualne, int *zostalo, int kierunek, bool *koniec)
{

    if (!*koniec) //jesli Rockford jest na planszy
    {
        int pom = plansza[aktualne->x][aktualne->y + kierunek]; //pom - symbol na polu docelowym
        pole cel;
        cel.x = aktualne->x;
        cel.y = aktualne->y + kierunek;
        //jesli jest mozliwy ruch na pole docelowe
        if (pom == puste || pom == ziemia || pom == diament ||
            (pom == kamien && plansza[cel.x][cel.y + kierunek] == puste))
        {
            if (pom == diament)
                *zostalo -= 1;
            if (pom == kamien)
                plansza[cel.x][cel.y + kierunek] = kamien;
            plansza[cel.x][cel.y] = Rockford;
            plansza[aktualne->x][aktualne->y] = puste;
            aktualne->y = cel.y;
        }
        if (pom == wyjscie && *zostalo == 0) //jesli Rockford może zakończyc grę
        {
            plansza[aktualne->x][aktualne->y] = puste;
            *koniec = 1;
        }
    }
}

//wczytuje i wykonuje polecenia uzytkownika:
// ruch w górę, ruch w dół, wuch w prawo, ruch w lewo, wypisanie planszy na ekranie
//gdy koniec=0 Rockford jest na planszy, gdy koniec=1 Rockforda nie ma na planszy
void polecenia(int **plansza, pole *aktualne, int *zostalo, int w, int k, bool *koniec)
{
    int gdzie = getchar();
    while (gdzie != EOF)
    {
        stabilny(w, k, plansza);
        if (gdzie == 10)
            wypisz_plansze(w, k, plansza);
        else if (gdzie == W)
            przesun_pionowo(plansza, aktualne, zostalo, -1, koniec);
        else if (gdzie == S)
            przesun_pionowo(plansza, aktualne, zostalo, 1, koniec);
        else if (gdzie == A)
            przesun_poziomo(plansza, aktualne, zostalo, -1, koniec);
        else //gdzie =D
            przesun_poziomo(plansza, aktualne, zostalo, 1, koniec);
        gdzie = getchar();
    }
}

//wypisuje planszę po raz pierwszy i wywołuję funkcję wykonującą polecenia uzytkownika
void wykonaj(int w, int k, int **plansza, pole *aktualne, int *zostalo, bool *koniec)
{
    stabilny(w, k, plansza);
    wypisz_plansze(w, k, plansza);
    polecenia(plansza, aktualne, zostalo, w, k, koniec);
}

//wczytuje planszę i rozpoczyna grę
void graj(int w, int k, int **plansza)
{
    getchar();
    bool koniec = 1;//jesli Rockforda nie ma na planszy to koniec=1
    pole aktualne;//pole na którym jest Rockford
    int zostalo = 0;//ile diamentów zostało jeszcze na planszy
    wczytaj_plansze(w, k, plansza, &aktualne, &zostalo, &koniec);
    wykonaj(w, k, plansza, &aktualne, &zostalo, &koniec);
}

//alokuje pamięć tablicy dwuwymiarowej o rozmiarze wXk
int **alokacja(int w, int k)
{
    int **tab;
    tab = calloc((size_t) w, sizeof(int *));//przydzielanie pamięci;
    for (int i = 0; i < w; i++)
        tab[i] = calloc((size_t) k, sizeof(int));
    return tab;
}

//zwalnia pamięć zaalokowaną w tablicy dwuwymiarowej tab
void zwolnij_pamiec(int **tab, int w)
{
    for (int i = 0; i < w; i++)
        free(tab[i]);
    free(tab);
}

int main() {
    int w;
    int k;
    if (scanf("%d%d", &w, &k))
    {
        int **tab;
        tab = alokacja(w, k);
        graj(w, k, tab);
        zwolnij_pamiec(tab, w);
    }
    return 0;
}