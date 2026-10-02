#ifndef KOSZTY_I_KONFIGURACJA_H
#define KOSZTY_I_KONFIGURACJA_H

namespace Rachunki {
	inline float cenaKWh = 1.20f;       // Koszt 1 kWh energii elektrycznej w PLN
	inline float cenaLitra = 0.02f;     // Koszt 1 litra wody w PLN
	inline float stawkaPodatku = 0.19f; // Podatek dochodowy (19%)

	inline const int KOSZT_NAPRAWY_PRALKI = 80;   // Koszt przywrócenia trwałości do 100% (PLN)
	inline const int SPADEK_TRWALOSCI_FULL = 5;   // Zużycie pralki po pełnym cyklu prania (%)
	inline const int SPADEK_TRWALOSCI_ANUL = 8;   // Zużycie pralki przy awaryjnym anulowaniu (%)

	inline const int KOSZT_KARY_ANULOWANIA = 15; // Dodatkowa opłata operacyjna/odszkodowanie przy anulowaniu (PLN)
}

#endif
