#ifndef PRALKA_H
#define PRALKA_H

#include "KosztyIKonfiguracja.h"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <algorithm>

//Zawiera Klasę Pralka oraz struktury do Prograamu prania i Ulepszenia. Logikę jednej pralki i uruchamiania prania w osobnym wątku. Tutaj liczone są też koszty mediów, podatek oraz zysk netto.


struct ProgramPrania {
	std::string nazwa;
	int temp;            // °C
	float maxWsadKg;     // kg
	int czasMinut;       // min
	float zuzycieWodyL;  // litry
	float zuzycieKWh;    // kWh
	int obroty;          // obr./min
	int cennikKlienta;   // sugerowana opłata od klienta (PLN)
};

// Struktura ulepszenia używana w sklepie i zapisie gry
struct Ulepszenie {
	int id;
	std::string nazwa;
	std::string opis;
	int koszt;
	bool kupione;
	float modyfikatorCzasu;  // mnożnik czasu (< 1.0 = skrócenie)
	float modyfikatorEko;    // mnożnik zużycia mediów (< 1.0 = oszczędność)
	int dodatkowyZysk;       // bonus PLN do przychodu
};

class Pralka {
	private:
		int id;
		std::atomic<bool> czyPracuje{false};
		std::atomic<bool> anulujFlaga{false};

		int trwalosc = 100;
		bool czyZepsuta = false;

		std::vector<ProgramPrania> bazaProgramow;

		float mnoznikCzasu = 1.0f;
		float mnoznikEko = 1.0f;
		int bonusZysk = 0;

	public:
		Pralka(int idPralki) : id(idPralki) {
			bazaProgramow = {
				{"Bawełna 90°",                      90, 6.0f, 180, 83.0f, 1.95f, 1000, 150},
				{"Bawełna 40°",                      40, 6.0f, 180, 81.0f, 1.00f, 1000, 120},
				{"Bawełna 20°",                      20, 6.0f, 180, 81.0f, 0.50f, 1000, 100},
				{"Bawełna 60° z Praniem Wstępnym",   60, 6.0f, 200, 92.0f, 1.80f, 1000, 160},
				{"Bawełna Eco 60° (Pełny wsad)",     60, 6.0f, 250, 47.5f, 0.86f, 1000, 140},
				{"Bawełna Eco 60° (Połowa wsadu)",   60, 3.0f, 195, 35.0f, 0.54f, 1000, 110},
				{"Bawełna Eco 40°",                  40, 3.0f, 195, 33.7f, 0.52f, 1000, 110},
				{"Syntetyki 40°",                    40, 2.5f, 120, 66.0f, 0.75f, 1000,  90},
				{"Syntetyki 20°",                    20, 2.5f, 120, 66.0f, 0.25f, 1000,  80},
				{"Codzienne Szybkie 30°",            30, 6.0f,  28, 60.0f, 0.15f, 1000,  60},
				{"Wełna / Pranie ręczne 40°",        40, 1.5f,  65, 45.0f, 0.45f, 1000,  75},
				{"Puchowe 40°",                      40, 1.5f, 105, 85.0f, 0.80f, 1000,  95},
				{"Koszule 40°",                      40, 2.5f,  80, 42.0f, 0.60f,  800,  85},
				{"Czyszczenie Bębna",                90, 0.0f, 160, 67.0f, 2.10f,  600,   0}
			};
		}

		bool jestWolna() const { return !czyPracuje; }
		bool czyAwarie() const { return czyZepsuta; }
		int getId() const { return id; }
		int getTrwalosc() const { return trwalosc; }
		const std::vector<ProgramPrania>& getBazaProgramow() const { return bazaProgramow; }

		void ustawUlepszenia(float modCzasu, float modEko, int bonus) {
			mnoznikCzasu = modCzasu;
			mnoznikEko = modEko;
			bonusZysk = bonus;
		}

		void ustawStanTechniczny(int nowaTrwalosc, bool zepsuta) {
			trwalosc = nowaTrwalosc;
			czyZepsuta = zepsuta;
		}

		void przerwijPranie() {
			if (czyPracuje) {
				anulujFlaga = true;
			}
		}

		bool napraw() {
			if (czyZepsuta || trwalosc < 100) {
				trwalosc = 100;
				czyZepsuta = false;
				return true;
			}
			return false;
		}

		// Uruchamia pranie asynchronicznie dla podanych parametrów (czas w sekundach).
		void uruchomPranieAsync(int czasSekund, int cenaKlient, float kWh, float litry, std::atomic<int>& portfel)
		{
			if (czyZepsuta) {
				std::cout << "\n Nie można uruchomić pralki #" << id << " - wymagana naprawa!\n";
				return;
			}

			czyPracuje = true;
			anulujFlaga = false;

			// Skala: 10 minut programu = 1 sekunda symulacji (600 s -> 1 s).
			int czasSymulacjiSekundy = std::max(1, static_cast<int>(czasSekund * mnoznikCzasu));
			float zuzycieKWh   = kWh   * mnoznikEko;
			float zuzycieWody  = litry * mnoznikEko;
			int   przychod     = cenaKlient + bonusZysk;

			std::thread([this, czasSymulacjiSekundy, przychod,
					zuzycieKWh, zuzycieWody, &portfel]() {
					int przepracowaneSekundy = 0;

					for (int i = 0; i < czasSymulacjiSekundy; ++i) {
					if (anulujFlaga) break;
					std::this_thread::sleep_for(std::chrono::seconds(1));
					przepracowaneSekundy++;
					}

					// PROCES ANULOWANY
					if (anulujFlaga) {
					float ulamekCzasu = static_cast<float>(przepracowaneSekundy)
					/ czasSymulacjiSekundy;
					float kosztPradu = (zuzycieKWh  * ulamekCzasu) * Rachunki::cenaKWh;
					float kosztWody  = (zuzycieWody * ulamekCzasu) * Rachunki::cenaLitra;
					int strataPrzeniesiona = static_cast<int>(kosztPradu + kosztWody);

					portfel -= strataPrzeniesiona;

					trwalosc = std::max(0, trwalosc - Rachunki::SPADEK_TRWALOSCI_ANUL);
					if (trwalosc == 0) czyZepsuta = true;

					std::cout << "\n\n [PRALKA #" << id << "] Pranie PRZERWANE przez użytkownika!"
						<< "\n - Przychód od klienta: 0 PLN (zwrot wpłaty)"
						<< "\n - Poniesiony koszt zużytych mediów: -"
						<< strataPrzeniesiona << " PLN"
						<< "\n - Aktualna trwałość pralki: " << trwalosc << "%\n> "
						<< std::flush;
					}
					else {
						float kosztPradu = zuzycieKWh  * Rachunki::cenaKWh;
						float kosztWody  = zuzycieWody * Rachunki::cenaLitra;
						float lacznyKosztMediow = kosztPradu + kosztWody;

						float dochodBrutto = przychod - lacznyKosztMediow;
						float podatek = (dochodBrutto > 0) ? (dochodBrutto * Rachunki::stawkaPodatku) : 0.0f;
						int zyskNetto = static_cast<int>(dochodBrutto - podatek);

						portfel += zyskNetto;

						trwalosc = std::max(0, trwalosc - Rachunki::SPADEK_TRWALOSCI_FULL);
						if (trwalosc == 0) {
							czyZepsuta = true;
						}

						std::cout << "\n\n [PRALKA #" << id << "] Pranie zakończone sukcesem!"
							<< "\n - Przychód: " << przychod << " PLN"
							<< "\n - Rachunki (Prąd + Woda): -" << lacznyKosztMediow << " PLN"
							<< "\n - Podatek dochodowy: -" << podatek << " PLN"
							<< "\n - Zysk NETTO: +" << zyskNetto << " PLN"
							<< "\n - Pozostała trwałość sprzętu: " << trwalosc << "%"
							<< (czyZepsuta ? " [AWARIA! Sprzęt wymaga naprawy]" : " ")
							<< "\n> " << std::flush;
					}

					czyPracuje = false;
					anulujFlaga = false;
					}).detach();
		}
};

#endif
