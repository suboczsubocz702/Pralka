#include <iostream>
#include <limits>
#include <cstdlib>            // dla system()
#include "PralniaManager.h"
#include "ZdarzeniaLosowe.h"
#include <iomanip>

//Plik zawiera pętlę menu, wybór programu prania, obsługę sklepu, read/write, oraz portfel i czyszczene ekranu

static void pokazPortfel(const PralniaManager& pralnia) {
	std::cout << "PORTFEL: " << std::setw(8) << pralnia.pobierzGotowke() << " PLN\n";
}

static void wyczyscEkran() {
#if defined(_WIN32) || defined(_WIN64)
	std::system("cls");
#else
	std::cout << "\033[2J\033[H" << std::flush;
#endif
}

static void czekajNaEnter() {
	std::cout << "\n [Enter aby kontynuować]";
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cin.get();
}

int main() {
#if defined(_WIN32) || defined(_WIN64)
	system("chcp 65001 > nul");
#endif

	ZdarzeniaLosowe::inicjalizuj();
	PralniaManager pralnia;

	bool dziala = true;
	while (dziala) {
		pokazPortfel(pralnia);
		std::cout << " AUTOMATYCZNA PRALNIA CPU (SIMULATOR)\n";
		std::cout << " 1. Uruchom pranie\n";
		std::cout << " 2. Podgląd stanu i naprawa pralek\n";
		std::cout << " 3. Anuluj pranie w trakcie\n";
		std::cout << " 4. Sklep (Ulepszenia & Nowe wątki CPU)\n";
		std::cout << " 5. Zapisz stan gry\n";
		std::cout << " 6. Wczytaj stan gry\n";
		std::cout << " 0. Wyjście\n";
		std::cout << " Wybór: ";

		char wybor;
		std::cin >> wybor;

		switch (wybor) {
			case '1': {
					  std::cout << "\n\t\t WYBÓR PROGRAMU PRANIA\n";
					  std::cout << " [1]  Bawełna 90°                    - Czas: 180min  | Cena: 150 PLN | Media: 1.95 kWh, 83.0 L\n";
					  std::cout << " [2]  Bawełna 40°                    - Czas: 180min  | Cena: 120 PLN | Media: 1.00 kWh, 81.0 L\n";
					  std::cout << " [3]  Bawełna 20°                    - Czas: 180min  | Cena: 100 PLN | Media: 0.50 kWh, 81.0 L\n";
					  std::cout << " [4]  Bawełna 60° z Praniem Wstępnym - Czas: 200min  | Cena: 160 PLN | Media: 1.80 kWh, 92.0 L\n";
					  std::cout << " [5]  Bawełna Eco 60° (Pełny wsad)   - Czas: 250min  | Cena: 140 PLN | Media: 0.86 kWh, 47.5 L\n";
					  std::cout << " [6]  Bawełna Eco 60° (Połowa wsadu) - Czas: 200min  | Cena: 110 PLN | Media: 0.54 kWh, 35.0 L\n";
					  std::cout << " [7]  Bawełna Eco 40°                - Czas: 195min  | Cena: 110 PLN | Media: 0.52 kWh, 33.7 L\n";
					  std::cout << " [8]  Syntetyki 40°                  - Czas: 120min  | Cena:  90 PLN | Media: 0.75 kWh, 66.0 L\n";
					  std::cout << " [9]  Syntetyki 20°                  - Czas: 120min  | Cena:  80 PLN | Media: 0.25 kWh, 66.0 L\n";
					  std::cout << " [10] Codzienne Szybkie 30°          - Czas: 28min   | Cena:  60 PLN | Media: 0.15 kWh, 60.0 L\n";
					  std::cout << " [11] Wełna / Pranie ręczne 40°      - Czas: 65min   | Cena:  75 PLN | Media: 0.45 kWh, 45.0 L\n";
					  std::cout << " [12] Puchowe 40°                    - Czas: 105min  | Cena:  95 PLN | Media: 0.80 kWh, 85.0 L\n";
					  std::cout << " [13] Koszule 40°                    - Czas: 80min   | Cena:  85 PLN | Media: 0.60 kWh, 42.0 L\n";
					  std::cout << " [14] Czyszczenie Bębna              - Czas: 160min  | Cena:   0 PLN | Media: 2.10 kWh, 67.0 L\n";
					  std::cout << " \nWybór (1-14) lub 0 aby anulować: ";

					  int prog;
					  std::cin >> prog;

					  bool sukces = false;
					  switch (prog) {
						  case 1:  sukces = pralnia.dodajPranie(180 * 60, 150, 1.95f, 83.0f); break;
						  case 2:  sukces = pralnia.dodajPranie(180 * 60, 120, 1.00f, 81.0f); break;
						  case 3:  sukces = pralnia.dodajPranie(180 * 60, 100, 0.50f, 81.0f); break;
						  case 4:  sukces = pralnia.dodajPranie(200 * 60, 160, 1.80f, 92.0f); break;
						  case 5:  sukces = pralnia.dodajPranie(250 * 60, 140, 0.86f, 47.5f); break;
						  case 6:  sukces = pralnia.dodajPranie(200 * 60, 110, 0.54f, 35.0f); break;
						  case 7:  sukces = pralnia.dodajPranie(195 * 60, 110, 0.52f, 33.7f); break;
						  case 8:  sukces = pralnia.dodajPranie(120 * 60,  90, 0.75f, 66.0f); break;
						  case 9:  sukces = pralnia.dodajPranie(120 * 60,  80, 0.25f, 66.0f); break;
						  case 10: sukces = pralnia.dodajPranie(28 * 60,   60, 0.15f, 60.0f); break;
						  case 11: sukces = pralnia.dodajPranie(65 * 60,   75, 0.45f, 45.0f); break;
						  case 12: sukces = pralnia.dodajPranie(105 * 60,  95, 0.80f, 85.0f); break;
						  case 13: sukces = pralnia.dodajPranie(80 * 60,   85, 0.60f, 42.0f); break;
						  case 14: sukces = pralnia.dodajPranie(160 * 60,   0, 2.10f, 67.0f); break;
						  default: break;
					  }

					  if (prog >= 1 && prog <= 14) {
						  if (sukces) {
							  std::cout << "\n Uruchomiono program prania!\n";
						  } else {
							  std::cout << "\n Nie udało się uruchomić prania. Wszystkie pralki są zajęte lub uszkodzone.\n";
						  }
					  }
					  break;
				  }

			case '2': {
					  pralnia.wyswietlStanPralek();
					  std::cout << "\n Podaj ID pralki do naprawy (lub 0 aby wrócić): ";
					  int idDoNaprawy;
					  std::cin >> idDoNaprawy;
					  if (idDoNaprawy > 0) {
						  pralnia.naprawPralke(idDoNaprawy);
					  }
					  break;
				  }

			case '3': {
					  pralnia.wyswietlStanPralek();
					  std::cout << "\n Podaj ID pralki, której pranie chcesz ANULOWAĆ (lub 0 aby wrócić): ";
					  int idDoAnulowania;
					  std::cin >> idDoAnulowania;
					  if (idDoAnulowania > 0) {
						  pralnia.anulujPranie(idDoAnulowania);
					  }
					  break;
				  }

			case '4': {
					  pralnia.wyswietlSklep();
					  std::cout << "\n Wybierz opcję (W - dokup wątek CPU, ID ulepszenia lub 0 aby wrócić): ";
					  char akcja;
					  std::cin >> akcja;

					  if (akcja == 'W' || akcja == 'w') {
						  if (pralnia.dokupWatek()) {
							  std::cout << "\n Odblokowano nowy wątek CPU!\n";
						  }
					  } else if (akcja >= '1' && akcja <= '9') {
						  if (pralnia.kupUlepszenie(akcja - '0')) {
							  std::cout << "\n Zakupiono ulepszenie!\n";
						  }
					  }
					  break;
				  }

			case '5':
				  if (pralnia.zapiszStanGry()) {
					  std::cout << "\n Gra została pomyślnie zapisana!\n";
				  } else {
					  std::cout << "\n Błąd zapisu gry.\n";
				  }
				  break;

			case '6':
				  if (pralnia.wczytajStanGry()) {
					  std::cout << "\n Stan gry został pomyślnie wczytany!\n";
				  } else {
					  std::cout << "\n Nie odnaleziono pliku zapisu.\n";
				  }
				  break;

			case '0':
				  dziala = false;
				  std::cout << "\n Dziękujemy za grę!\n";
				  break;

			default:
				  std::cout << "\n Nieprawidłowy wybór. Spróbuj ponownie.\n";
				  break;
		}
		if (dziala) {
			czekajNaEnter();
			wyczyscEkran();
		}
	}
	return 0;
}
