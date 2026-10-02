#ifndef PRALNIA_MANAGER_H
#define PRALNIA_MANAGER_H

#include "Pralka.h"
#include "KosztyIKonfiguracja.h"
#include "ZapisGry.h"
#include "ZdarzeniaLosowe.h"
#include <vector>
#include <memory>
#include <thread>
#include <iostream>
#include <algorithm>
#include <atomic>

//Trzyma portfel, listę pralek, sklep z ulepszeniami, limit wątków CPU. Otwiera menu z opcjami.

class PralniaManager {
	private:
		std::atomic<int> gotowka{300};

		// Kolejność pól musi odpowiadać kolejności w liście inicjalizacyjnej,
		// aby uniknąć ostrzeżeń -Wreorder.
		const int fizycznaLiczbaWatkow;
		int maxWatkow;
		int kosztNowegoWatku;

		std::vector<std::unique_ptr<Pralka>> pralki;
		std::vector<Ulepszenie> sklepUlepszen;
		const std::string NAZWA_PLIKU_ZAPISU = "SaveManager.txt";

	public:
		PralniaManager()
			: fizycznaLiczbaWatkow(
					std::thread::hardware_concurrency() == 0
					? 1
					: static_cast<int>(std::thread::hardware_concurrency())),
			maxWatkow(1),
			kosztNowegoWatku(150)
			{
				sklepUlepszen = {
					{1, "Mikroprocesor", "Skraca czas prania o 5%",
						120, false, 0.95f, 1.0f, 0},
					{2, "Izolacja Akustyczna", "Skraca czas prania o kolejne 4%",
						150, false, 0.96f, 1.0f, 0},
					{3, "Czujnik Mętności Wody", "Zmniejsza zużycie mediów o 7%",
						180, false, 1.0f, 0.93f, 0},
					{4, "Automatyczny Dozownik", "Niewielki bonus do wpłaty +10 PLN",
						200, false, 1.0f, 1.0f, 10},
					{5, "System Antywibracyjny", "Zmniejsza zużycie sprzętu o 15%",
						250, false, 1.0f, 1.0f, 0}
				};
				zsynchronizujWatki();
			}

		int pobierzGotowke() const {
			return gotowka.load();
		}

		bool dodajPranie(int czasSekund, int cenaKlient, float kWh, float litry) {
			for (auto& p : pralki) {
				if (p->jestWolna() && !p->czyAwarie()) {
					p->ustawUlepszenia(pobierzModyfikatorCzasu(),
							pobierzModyfikatorEko(),
							pobierzBonusZysku());
					p->uruchomPranieAsync(czasSekund, cenaKlient, kWh, litry, gotowka);
					ZdarzeniaLosowe::sprawdzIWykonaj(gotowka);
					return true;
				}
			}
			std::cout << "\n Wszystkie pralki są zajęte lub uszkodzone.\n";
			return false;
		}

		bool anulujPranie(int idPralki) {
			for (auto& p : pralki) {
				if (p->getId() == idPralki) {
					if (!p->jestWolna()) {
						p->przerwijPranie();
						std::cout << "\n Pranie w pralce #" << idPralki
							<< " zostało anulowane,\n strata opłat za prąd, wodę i usługę.";
						return true;
					} else {
						std::cout << "\n Pralka #" << idPralki << " obecnie nie pracuje.";
						return false;
					}
				}
			}
			std::cout << "\n Nie znaleziono pralki o podanym ID.\n";
			return false;
		}

		bool naprawPralke(int idPralki) {
			for (auto& p : pralki) {
				if (p->getId() == idPralki) {
					int kosztNaprawy = Rachunki::KOSZT_NAPRAWY_PRALKI;
					if (gotowka >= kosztNaprawy) {
						if (p->napraw()) {
							gotowka -= kosztNaprawy;
							std::cout << "\n Pralka #" << idPralki
								<< " została naprawiona. Odjęto "
								<< kosztNaprawy << " PLN.\n";
							return true;
						}
					} else {
						std::cout << "\n Brak wystarczających środków na naprawę. Potrzebne "
							<< kosztNaprawy << " PLN.\n";
						return false;
					}
				}
			}
			return false;
		}

		bool dokupWatek() {
			if (maxWatkow >= fizycznaLiczbaWatkow) {
				std::cout << "\n Osiągnięto limit! Twój procesor posiada tylko "
					<< fizycznaLiczbaWatkow << " wątków sprzętowych.\n";
				return false;
			}
			if (gotowka >= kosztNowegoWatku) {
				gotowka -= kosztNowegoWatku;
				maxWatkow++;
				zsynchronizujWatki();
				kosztNowegoWatku = static_cast<int>(kosztNowegoWatku * 1.6);
				return true;
			}
			std::cout << "\n Za mało gotówki na odblokowanie kolejnego wątku!\n";
			return false;
		}

		void wyswietlSklep() const {
			std::cout << "\nSKLEP I SPRZĘT\n";
			std::cout << " Stan konta: " << gotowka << " PLN\n";
			std::cout << " Odblokowane wątki CPU: " << maxWatkow
				<< " / " << fizycznaLiczbaWatkow
				<< " (Fizyczny limit CPU)\n";

			if (maxWatkow < fizycznaLiczbaWatkow) {
				std::cout << " [W] Dokup 1 Wątek CPU (Nowa pralka) - Cena: "
					<< kosztNowegoWatku << " PLN\n";
			} else {
				std::cout << " [W] [MAKSYMALNA LICZBA WĄTKÓW OSIĄGNIĘTA]\n";
			}

			std::cout << "\n Modyfikatory: Czas: "
				<< (1.0f - pobierzModyfikatorCzasu()) * 100.0f
				<< "% | Eko: "
				<< (1.0f - pobierzModyfikatorEko()) * 100.0f << "%\n";

			for (const auto& u : sklepUlepszen) {
				std::cout << " [" << u.id << "] " << u.nazwa << " - " << u.koszt
					<< " PLN | " << u.opis << " ["
					<< (u.kupione ? "KUPIONE" : "DOSTĘPNE") << "]\n";
			}
		}

		bool kupUlepszenie(int id) {
			for (auto& u : sklepUlepszen) {
				if (u.id == id) {
					if (u.kupione) {
						std::cout << "\n To ulepszenie zostało już kupione.\n";
						return false;
					}
					if (gotowka >= u.koszt) {
						gotowka -= u.koszt;
						u.kupione = true;
						return true;
					}
				}
			}
			std::cout << "\n Brak środków lub błędne ID ulepszenia.\n";
			return false;
		}

		void wyswietlStanPralek() const {
			std::cout << "\n STAN SYSTEMU / PRALEK \n";
			for (const auto& p : pralki) {
				std::cout << " Pralka #" << p->getId()
					<< " | Stan: " << (p->jestWolna() ? "WOLNA " : "PRACUJE")
					<< " | Zużycie sprzętu: " << (100 - p->getTrwalosc()) << "%"
					<< (p->czyAwarie() ? " [AWARIA!]" : "") << "\n";
			}
		}

		bool zapiszStanGry() {
			return ZapisGry::zapisz(NAZWA_PLIKU_ZAPISU, gotowka.load(),
					maxWatkow, sklepUlepszen, pralki);
		}

		bool wczytajStanGry() {
			int tymczasowaGotowka = gotowka.load();
			if (ZapisGry::wczytaj(NAZWA_PLIKU_ZAPISU, tymczasowaGotowka,
						maxWatkow, sklepUlepszen, pralki)) {
				gotowka = tymczasowaGotowka;
				if (maxWatkow > fizycznaLiczbaWatkow) {
					maxWatkow = fizycznaLiczbaWatkow;
				}
				zsynchronizujWatki();
				return true;
			}
			return false;
		}

	private:
		void zsynchronizujWatki() {
			while (static_cast<int>(pralki.size()) < maxWatkow) {
				pralki.push_back(
						std::make_unique<Pralka>(static_cast<int>(pralki.size()) + 1));
			}
		}

		float pobierzModyfikatorCzasu() const {
			float mod = 1.0f;
			for (const auto& u : sklepUlepszen) {
				if (u.kupione && u.modyfikatorCzasu < 1.0f) {
					mod *= u.modyfikatorCzasu;
				}
			}
			return mod;
		}

		float pobierzModyfikatorEko() const {
			float mod = 1.0f;
			for (const auto& u : sklepUlepszen) {
				if (u.kupione && u.modyfikatorEko < 1.0f) {
					mod *= u.modyfikatorEko;
				}
			}
			return mod;
		}

		int pobierzBonusZysku() const {
			int bonus = 0;
			for (const auto& u : sklepUlepszen) {
				if (u.kupione) bonus += u.dodatkowyZysk;
			}
			return bonus;
		}
};

#endif
