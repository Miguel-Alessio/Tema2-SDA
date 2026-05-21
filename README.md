# Tema 2 SDA

**Nume:** Pasagali Popa Miguel Alessio
**Grupă:** 315CC
**Materie:** Structuri de Date și Algoritmi  

## Descriere Generală
Această temă implementează un sistem eficient de indexare și căutare a fișierelor text pe baza unor cuvinte cheie. Proiectul simulează comportamentul unui motor de căutare de bază, capabil să adauge, șteargă și să găsească documente rapid, inclusiv să sugereze cele mai relevante `k` rezultate (TOPK) sau să facă autocomplete (PREFIX). 

Toată logica se bazează pe trei structuri de date fundamentale: **Lista Dublu Înlănțuită**, **Trie (Arbore Multicăi)** și **Max/Min-Heap**.

## Structuri de Date Folosite

1. **Lista Dublu Înlănțuită (FileNode)**
   * Păstrează baza de date a fișierelor într-un mod simplu de iterat și șters.
   * Fiecare nod reține ID-ul fișierului, scorul de relevanță, numărul de cuvinte și un array bidimensional (alocat dinamic) de cuvinte cheie.

2. **Arborele Trie (TrieNode)**
   * Este baza sistemului de căutare. Permite găsirea unui cuvânt în timp O(n), unde n este lungimea cuvântului, complet independent de numărul total de cuvinte din sistem.
   * Fiecare nod din Trie care marchează finalul unui cuvânt (end_of_word = true) deține și o listă simplu înlănțuită (FileRefNode) cu referințe către fișierele din Lista Dublă care conțin acel cuvânt.

3. **Heap (Heap)**
   * Folosit exclusiv pentru comanda TOPK.
   * În loc să sortez toate fișierele returnate de o căutare (ceea ce ar fi costisitor), inserez elementele într-un Heap, prioritatea fiind dată de scorul fișierului (și lexicografic în caz de egalitate). Apoi extrag doar primele k elemente prin operatii de heapifyDown.

## Detalii de Implementare & Funcționalități

* **ADD / DEL:** Adăugarea creează un fișier nou și populează Trie-ul. La ștergere, pe lângă eliminarea fișierului din listă, se parcurge Trie-ul și se elimină doar referința către fișierul curent. Nodurile din Trie sunt șterse complet doar dacă nu mai au copii și nu mai sunt capăt de cuvânt.
* **ADDKW / DELKW:** Modifică local cuvintele cheie din structura FileNode a fișierului țintă și actualizează legăturile din Trie.
* **FIND:** Caută un cuvânt în Trie. Dacă ajunge la finalul cuvântului, preia toate fișierele asociate, le pune într-un array de pointeri și le sortează lexicografic eficient folosind qsort.
* **PREFIX:** Parcurge Trie-ul până termină prefixul dat, apoi realizează o parcurgere recursivă (DFS) în subarborele respectiv pentru a aduna și a sorta toate fișierele unice care conțin cuvinte ce încep cu acel prefix.

## Dificultăți Întâmpinate și Soluții

1.  **Memory Limit / Segmentation Fault pe testele mari:** Inițial am încercat să aloc static pe stivă vectori uriași pentru sortarea rezultatelor (ex. char nume_files[25000][50]). Această abordare crăpa pe unele teste. Soluția a fost să salvez doar *pointeri* către ID-urile fișierelor (`const char**`) și să număr elementele înainte de a face un malloc curat pe heap. Astfel, consumul a scăzut drastic.
2.  **Mesajele de eroare (EMPTY vs NOT FOUND):** A fost nevoie de mare atenție la enunț: NOT FOUND este pentru obiecte care lipsesc când încercăm să modificăm structura (la DEL, ADDKW), iar EMPTY este exclusiv un răspuns pentru o căutare fără rezultat (FIND, TOPK, PREFIX).
3.  **Parsarea Input-ului:** Unele fișiere de test .in aveau un număr de comenzi (n_ops) declarat pe prima linie care nu se pupa mereu cu numărul real de comenzi din interior. Am rezolvat problema adăugând un mecanism de fallback pe verificarea valorii returnate de scanf, combinat cu execuția forțată a ultimei operații, pentru a garanta sincronizarea cu checker-ul.
