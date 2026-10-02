#ifndef ZAPIS_GRY_H
#define ZAPIS_GRY_H

#include "KosztyIKonfiguracja.h"
#include "Pralka.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

//Serializuje do pliku tekstowego SaveManager.txt: tj. gotówk, liczbę wątków, ceny mediów, stan ulepszeń i stan techniczny każdej pralki

class ZapisGry {
	public:
		// Zapisuje pełny stan rozgrywki do pliku tekstowego
		static bool zapisz(const std::string& sciezka, 
				int gotowka, 
				int maxWatkow, 
				const std::vector<Ulepszenie>& sklep,
				const std::vector<std::unique_ptr<Pralka>>& pralki) 
		{
			std::ofstream plik(sciezka);
			if (!plik.is_open()) return false;

			// 1. Podstawowe statystyki finansowe i sprzętowe
			plik << gotowka << "\n";
			plik << maxWatkow << "\n";

			// 2. Aktualne zmienne ekonomiczne (modyfikowane przez zdarzenia losowe)
			plik << Rachunki::cenaKWh << "\n";
			plik << Rachunki::cenaLitra << "\n";

			// 3. Stan ulepszeń w sklepie (ID oraz flaga: 1 = kupione, 0 = niekupione)
			plik << sklep.size() << "\n";
			for (const auto& u : sklep) {
				plik << u.id << " " << (u.kupione ? 1 : 0) << "\n";
			}

			// 4. Stan techniczny każdej z pralek (ID, Trwałość %, Czy zepsuta: 1/0)
			plik << pralki.size() << "\n";
			for (const auto& p : pralki) {
				plik << p->getId() << " " << p->getTrwalosc() << " " << (p->czyAwarie() ? 1 : 0) << "\n";
			}

			plik.close();
			return true;
		}

		// Wczytuje stan gry z pliku
		static bool wczytaj(const std::string& sciezka, 
				int& gotowka, 
				int& maxWatkow, 
				std::vector<Ulepszenie>& sklep,
				std::vector<std::unique_ptr<Pralka>>& pralki) 
		{
			std::ifstream plik(sciezka);
			if (!plik.is_open()) return false;

			// 1. Wczytanie podstawowych danych
			plik >> gotowka;
			plik >> maxWatkow;

			// 2. Wczytanie stawek za media
			plik >> Rachunki::cenaKWh;
			plik >> Rachunki::cenaLitra;

			// 3. Wczytanie stanu sklepu
			size_t liczbaUlepszen;
			if (plik >> liczbaUlepszen) {
				for (size_t i = 0; i < liczbaUlepszen; ++i) {
					int id, kupioneInt;
					plik >> id >> kupioneInt;
					for (auto& u : sklep) {
						if (u.id == id) {
							u.kupione = (kupioneInt == 1);
						}
					}
				}
			}

			// 4. Wczytanie stanu technicznego pralek
			size_t liczbaPralek;
			if (plik >> liczbaPralek) {
				// Czyścimy obecny wektor pralek i odtwarzamy je na podstawie zapisu
				pralki.clear();
				for (size_t i = 0; i < liczbaPralek; ++i) {
					int id, trwalosc, zepsutaInt;
					plik >> id >> trwalosc >> zepsutaInt;

					auto pralka = std::make_unique<Pralka>(id);
					pralka->ustawStanTechniczny(trwalosc, zepsutaInt == 1);
					pralki.push_back(std::move(pralka));
				}
			}

			plik.close();
			return true;
		}
};

#endif
