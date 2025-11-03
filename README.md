# IR-seeker
## Descrizione
Il progetto consiste nella realizzazione di un cercatore di fonti di calore (come una fiamma di un accendino), in grado di
tracciare la suddetta fonte sugli assi orizzontale e verticale.
Questo risultato è ottenuto con l'ausilio di 2 servo motori (1 per asse), pilotati in closed loop grazie alle letture ottenute da 4 moduli flame sensor (fototransistor con filtro ir).
I sensori sono disposti in modo da avere una lettura analogica per ogni direzione: sopra, sinistra, destra, sotto:
```
     O              O = sensore
     |
O----|----O
     |
     O
```
## Componenti principali
* Elegoo UNO r3
* 2 servo motori Miuzei mf90
* Alimentatore 6V 2A da collegare a presa elettrica 230V in corrente alternata
* 4 Youmile modulo sensore fiamma 4 pin
* Breadboard

## Schema elettrico
I servo motori hanno una corrente di stallo di 860 mA ±10% ciascuno, se alimentati a 6V, per cui sono collegati all'alimentatore apposito.
I sensori consumano poche decine di mA, per cui sono alimentati direttamente dalla board.
La board è alimentata direttamente dal connettore USB, collegato a un laptop.
![alt text](<electric schema.png>)

## Risultati

https://github.com/user-attachments/assets/d2c186e9-7a35-4e26-9c8a-cc57160b2427


## Difficoltà riscontrate
### Sistema di controllo
Il primo approccio per comandare il segnale da inviare ai motori è stato quello di usare due controllori PID (uno per motore).
Sottraendo i valori ottenuti dai sensori sinistra e destra si ottiene un errore sull'asse orizzontale, analogamente per gli altri due sensori, si ottiene
un errore sull'asse verticale.
Quando la fiamma è centrata, le coppie di sensori dovrebbero vedere la stessa intensità di luce(adc 10 bit, valori da 0 a 1023), e quindi dare un errore 0 o vicino a 0.
Per evitare bias dovuti ai valori assoluti, che cambiano in base alla distanza della fiamma, i 2 errori vengono normalizzati,
dividendo per la somma delle rispettive rilevazioni (per l'asse orizzontale l'errore è = (destra-sinistra)/(destra+sinistra)).
Dato il nuovo range di valori (da -1 a 1), alcuni accorgimenti sono stati necessari per evitare di pilotare i motori erroneamente:
* Quando i valori di intensità sono molto bassi, anche piccole variazioni si traducono in valori relativamente grandi di errore (es: 4-2/6 = 0.33).
La soluzione è stata quella di impostare l'errore a 0 se la somma delle rilevazioni è minore di 40.
* Empiricamente è stato rilevato che errori (in valore assoluto) minori di 0.4 sono dovuti a rumore e/o richiederebbero movimenti troppo fini del motore,
portando a oscillazioni. Per questo se gli errori sono minori di questa soglia, vengono considerati come 0.

Da subito l'inserimento della componente derivativa nel PID ha portato a grandi oscillazioni o a risultati non dissimili da un uso della sola componente proporzionale,
il che ha portato a esplorare meglio solo diversi valori del coefficiente P.
I risultati non sono stati comunque soddisfacenti, dato che per ottenere una reattività accettabile servivano valori alti del coefficiente, che portavano in alcuni casi
a oscillazioni. D'altro canto, valori più bassi annullavano le oscillazioni, ma rallentavano la reattività.
Osservando empiricamente i valori assunti dall'errore al variare della posizione della fiamma, è stato notato che la funzione che mette in relazione l'errore e 
la variazione di posizione non è lineare: sarebbe auspicabile una variazione di 1 o 2 gradi per valori dell'errore bassi (0.4-0.6), ed una variazione
di 6 o 7 gradi per errori alti (0.7-0.85).
Per questo è stata progettata una funzione per calcolare la variazione di posizione dei servo motori così definita:
* 100^(x-0.4) con x > 0
* 0 con x = 0
* -(1/100)^(x+0.4) con x < 0

I due esponenziali (uno per gli incrementi, l'altro per i decrementi di posizione), traslati di 0.4 (il minimo valore che è considerato come segnale affidabile),
risolvono i problemi delle oscillazioni, garantendo anche una buona reattività.
### Tracciamento negli angoli
Data la configurazione geometrica in cui sono posti i sensori, una fiamma che si trova in uno dei 4 angoli porterà a errori bassi.

```
 A    O    A          O = sensore
      |
 O----|----O          A = angolo
      |
 A    O    A
```
Una soluzione interessante sarebbe stata quella di disporre i sensori a forma di quadrato, calcolando gli errori in questo modo:
* errore x = somma sensori a destra - somma sensori a sinistra
* errore y = somma sensori in alto - somma sensori in basso

```
O------O
|      |
|      |
O------O   
```

In questo modo si avrebbero valori di errore consoni su tutta l'area 2D del quadrato.
Tuttavia, quando il problema è stato notato, la struttura geometrica era già stata realizzata, e i sensori erano già stati incollati.
Perciò, si è optato per una soluzione software.
Nella configurazione a "+", se una fiamma si trova negli angoli, gli errori sui due assi dovrebbero avere valori vicini (in valore assoluto).
Perciò, se gli errori in valore assoluto sono maggiori di 0.25, e la differenza dei loro valori assoluti è compresa tra -0.25 e 0.25,
allora vengono moltiplicati per 1.3.
Questo permette di avere una buona risposta anche per movimenti in diagonale, preservando comunque il comportamento originario del sistema.
