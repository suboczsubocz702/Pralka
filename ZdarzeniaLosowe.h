#ifndef ZDARZENIA_LOSOWE_H
#define ZDARZENIA_LOSOWE_H

#include "KosztyIKonfiguracja.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <atomic>

//Plik zawiera losowe eventy, ok 20% szans przy każdym praniu

class ZdarzeniaLosowe {
	public:
		static void inicjalizuj() {
			std::srand(static_cast<unsigned int>(std::time(nullptr)));
		}

		// Wywoływane z określoną szansą po uruchomieniu prania lub nowej turze
		static void sprawdzIWykonaj(std::atomic<int>& gotowka) {
			int los = std::rand() % 100; // Losowanie wartości 0-99

			if (los >= 20) {
				return;
			}

			if (los < 3) { // 4% szansy
				std::cout << "\n [EVENT] SKOK NAPIĘCIA! Ceny prądu rosną o 0,2% przez kryzys energetyczny.\n";
				Rachunki::cenaKWh *= 1.02f;
			} 
			else if (los >= 3 && los < 6) { // 4% szansy
				std::cout << "\n [EVENT] AWARIA SIEĆ WODOCIĄGOWEJ! Woda drożeje o 20%.\n";
				Rachunki::cenaLitra *= 1.20f;
			} 
			else if (los >= 5 && los < 9) { // 4% szansy
				std::cout << "\n [EVENT] KONTROLA SKARBOWA! Wykryto drobne błędy w papierach. Mandat: -100 PLN.\n";
				gotowka -= 100;
			} 
			else if (los >= 9 && los < 13) { // 5% szansy
				std::cout << "\n [EVENT] HOJNY KLIENT! Niezwykle zadowolony klient zostawia duży napiwek: +80 PLN.\n";
				gotowka += 80;
			} 
			else if (los >= 13 && los < 17) { // 5% szansy
				std::cout << "\n [EVENT] DOTACJA EKO! Otrzymujesz lokalne subsydium ekologiczne: +150 PLN.\n";
				gotowka += 150;
			}
			else if (los >= 17 && los < 20) { // 3% szansy
				std::cout << "\n [EVENT] PROMOCJA DLA FIRM! Zniżka na hurtowy zakup energii. Prąd tanieje o 10%.\n";
				Rachunki::cenaKWh *= 0.90f;
			}
		}
};

#endif
