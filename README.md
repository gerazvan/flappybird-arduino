# Documentation

-> For this project I've used the following components: LED matrix 8x8, MAX7219 module, one button and a 4 digits 7 segments display.

-> The game has 3 states: start, the actual game and end. Start and end states await for the user input. The start state displays the highest score obtained after the Arduino board was powered on, while the end states displays the score obtained throughout the last game played.

-> The game's difficulty grows progressively every 15 seconds for the maximum of 3 times. The difficulty is displayed using the DP leds from the 4D7S display: first LED from left to right represents the lowest difficulty, whereas the last LED from left to right represents the highest difficulty.

-> The score is incremented when a wall disappears from the matrix, meaning the 'bird' dodged it.

-> For playing this game, only the button is used. When pressed, the 'bird' goes up by 2 rows (in the matrix). If the 'bird' doesn't receive any input, it will go down in rows at constant pace. The game is over when the 'bird' hits a wall or when it goes under the last row of the matrix. The game ends as well when the player hits the score 9000 and it is considered that he won.

[Link to video gameplay hosted on Google Drive](https://photos.app.goo.gl/LnT4gBmRFhaEv0qA2)
-> I've played for 1 minute, therefore gameplay is shown throughout all difficulties.


----------------------------------------------------

# Documentatie

-> Pentru crearea acestui joc am folosit urmatoarele componente: matricea de LED-uri 8x8, modulul MAX7219, un buton si un display 4 digiti 7 segmente(am schimbat displayul LCD cu acesta).

-> Jocul are 3 stari: inceput, jocul propriu-zis si sfarsit. Starile de inceput si sfarsit asteapta input de la utilizator. Starea de inceput afiseaza scorul maxim obtinut de cand placa Arduino a fost alimentata. Starea de final afiseaza scorul obtinut in timpul ultimului joc.

-> Jocul acopera cerintele, acesta avand dificultate si scor. 
Dificultatea jocului creste progresiv, odata la 15 secunde, si de maxim 3 ori. Dificultatea este si ea afisata pe display in timpul jocului, aceasta fiind reprezentata folosind ledurile DP astfel: de la stanga spre dreapta, primul LED DP reprezinta prima dificultate; dupa 15s, cand dificultatea se schimba, primul LED se stinge si este aprins cel de al doilea; in final, dupa 60s, va ramane aprins al 4lea LED pana la sfarsitul jocului, acesta reprezentand ultima dificultate.
Scorul este incrementat atunci cand un zid este distrus, reprezentand faptul ca ‘pasarea’ s-a ferit de acesta.

-> Pentru a juca, este nevoie de un singur buton. La apasarea acestuia, ‘pasarea’ urca 2 linii. Cand pasarea nu primeste input pentru urcarea, aceasta coboara linii cu viteza constanta. Viteza de urcare este putin mai mare decat cea de coborare. Jocul se termina atunci cand ‘pasarea’ se loveste de un perete sau atunci cand este lasata sa coboare sub ultima linie. De asemenea, jocul se termina cand jucatorul atinge scorul de 9000, pentru a evita depasirea unei valori ce poate fi afisata cu 4 digiti. Practic, acesta este momentul in care jucatorul “castiga”.


[Link catre gameplay video hostat in Google Drive](https://photos.app.goo.gl/LnT4gBmRFhaEv0qA2)
-> Am reusit sa joc timp de 1 minut, astfel am prins 15s la ultima dificultate.
