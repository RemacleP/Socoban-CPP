#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <conio.h>
#include <windows.h>

struct Position { int x, y; };

// Affichage avec la hauteur et la largeur dynamiques
void afficherCarte(const std::vector<std::string>& carte) {
    system("cls");
    for (const std::string& ligne : carte) {
        std::cout << ligne << "\n";
    }
    std::cout << "\nZ:Haut, S:Bas, Q:Gauche, D:Droite, ESC:Quitter\n";
}

Position trouverJoueur(const std::vector<std::string>& carte, int h, int w) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (carte[y][x] == '@') return { x, y };
        }
    }
    return { -1, -1 };
}

int verifierVictoire(const std::vector<std::string>& carte, int h, int w) {
    int caissesSimples = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (carte[y][x] == '$') caissesSimples++;
        }
    }
    return (caissesSimples == 0);
}

int estCoinceeDansCoin(const std::vector<std::string>& carte, int x, int y, int h, int w) {
    if (carte[y][x] == '*') return 0;
    // Vérifier les limites pour éviter les dépassements
    int haut = (y > 0 && carte[y - 1][x] == '#');
    int bas = (y < h - 1 && carte[y + 1][x] == '#');
    int gauche = (x > 0 && carte[y][x - 1] == '#');
    int droite = (x < w - 1 && carte[y][x + 1] == '#');
    return ((haut || bas) && (gauche || droite));
}

// Fonction pour compter les niveaux et récupérer leurs tailles
int compterTotalNiveaux() {
    std::ifstream f("niveaux.txt");
    if (!f.is_open()) return 0;

    int h, l, total = 0;
    std::string buffer;
    while (f >> h >> l) {
        std::getline(f, buffer); // Consomme la fin de ligne du scanf/>>
        for (int i = 0; i < h; i++) {
            std::getline(f, buffer);
        }
        total++;
    }
    return total;
}

// Charger un niveau de taille variable en utilisant std::vector
int chargerNiveau(int index, std::vector<std::string>& carte, int* h, int* w) {
    std::ifstream f("niveaux.txt");
    if (!f.is_open()) return 0;

    int currentH, currentW, n = 0;
    std::string buffer;

    while (f >> currentH >> currentW) {
        std::getline(f, buffer);

        if (n == index) {
            *h = currentH;
            *w = currentW;
            carte.clear();

            // Lecture des lignes du niveau
            for (int i = 0; i < *h; i++) {
                std::getline(f, buffer);
                // Nettoyage potentiel des retours chariot de fin de ligne
                if (!buffer.empty() && buffer.back() == '\r') {
                    buffer.pop_back();
                }
                carte.push_back(buffer);
            }
            return 1;
        }
        else {
            for (int i = 0; i < currentH; i++) {
                std::getline(f, buffer);
            }
        }
        n++;
    }
    return 0;
}

int main() {
    std::vector<std::string> carte;
    int h = 0, w = 0;
    int niveau = 0;
    int totalNiveaux = compterTotalNiveaux();

    if (totalNiveaux == 0) {
        std::cout << "Erreur : Impossible de lire le fichier niveaux.txt !\n";
        _getch();
        return 1;
    }

    while (chargerNiveau(niveau, carte, &h, &w)) {
        int enJeu = 1;
        char derniereCase = ' ';

        while (enJeu) {
            afficherCarte(carte);
            if (verifierVictoire(carte, h, w)) {
                afficherCarte(carte);

                if (niveau >= totalNiveaux - 1) {
                    std::cout << "\nBravo ! Niveau termine !\n";
                    std::cout << "\nFelicitations ! Vous avez fini tous les niveaux !\n";
                    std::cout << "Fermeture automatique dans 3 secondes...";
                    Sleep(3000);
                    return 0;
                }

                std::cout << "\nBravo ! Niveau termine ! Passage au niveau suivant dans 3 secondes...";
                Sleep(3000);
                niveau++;
                enJeu = 0;
                break;
            }

            Position j = trouverJoueur(carte, h, w);
            char t = _getch();
            if (t == 27) {
                return 0; // ESC pour quitter
            }

            int nx = j.x, ny = j.y;
            if (t == 'z' || t == 'Z') ny--;
            else if (t == 's' || t == 'S') ny++;
            else if (t == 'q' || t == 'Q') nx--;
            else if (t == 'd' || t == 'D') nx++;

            // Sécurité pour ne pas sortir des limites du tableau
            if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;

            char caseVisee = carte[ny][nx];

            if (caseVisee == ' ' || caseVisee == '.') {
                carte[j.y][j.x] = derniereCase;
                derniereCase = (caseVisee == '.') ? '.' : ' ';
                carte[ny][nx] = '@';
            }
            else if (caseVisee == '$' || caseVisee == '*') {
                int cx = nx + (nx - j.x);
                int cy = ny + (ny - j.y);

                // Vérifier que la caisse poussée reste dans les limites
                if (cx >= 0 && cx < w && cy >= 0 && cy < h) {
                    char caseArriereCaisse = carte[cy][cx];

                    if (caseArriereCaisse == ' ' || caseArriereCaisse == '.') {
                        char nouveauSymboleCaisse = (caseArriereCaisse == '.') ? '*' : '$';

                        carte[cy][cx] = nouveauSymboleCaisse;
                        carte[ny][nx] = '@';

                        if (caseVisee == '*') {
                            carte[j.y][j.x] = '.';
                        }
                        else {
                            carte[j.y][j.x] = derniereCase;
                        }

                        derniereCase = ' ';

                        if (nouveauSymboleCaisse == '$' && estCoinceeDansCoin(carte, cx, cy, h, w)) {
                            afficherCarte(carte);
                            std::cout << "\nDefaite ! Caisse coincee. Fermeture dans 3 secondes...";
                            Sleep(3000);
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 0;
}