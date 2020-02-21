//headers per input/output, uso di stringhe e vettori e manipolazione files

#include <iostream>                     
#include <string>
#include <fstream>
#include "math.h"
#include "vector"

using namespace std;

float D = 0;                          //dimensione matrice
                           
vector<vector<char>> mat;           //matrice dinamica di gioco
vector<vector<vector<int>>> mat3D;  //matrice dinamica 3D, la terza dimensione indica i percorsi da
                                    //evitare(portano a vicoli cechi o a posizioni in cui si è già
                                    //passati
int i=0;                            //indici per coordinate matrici
int j=0;
string paroladiz;                   //stringa a cui viene assegnata ogni singola parola del
                                    //che viene esaminata
vector <int> righe;                 //vettori per le coordinate di ogni singola parola
vector <int> colonne;               

typedef struct node {               //struct per ogni singolo "blocco" da stampare; ogni blocco
                                    //è composto da: parola trovata, punteggio, percorso
    string parola;                  //(+ puntatori di collegamento
    
    decltype(paroladiz.size()) punti = 0;
    
    struct node* nextptr;
    
    vector<int> r;
    
    vector<int> c;
    
    
} node;

//prototipi di tutte le funzioni utilizzate nel programma

decltype(paroladiz.size()) punt(const string parola, decltype(parola.size()));
bool presente(const vector<int>, const vector<int>, int, int);
bool cercaprimo(vector<vector<char>>, int, int);
bool cercaw(vector<vector<char>>, const string, int, int, int);
bool cercasw(vector<vector<char>>, const string, int, int, int);
bool cercas(vector<vector<char>>, const string, int, int, int);
bool cercase(vector<vector<char>>, const string, int, int, int);
bool cercae(vector<vector<char>>, const string, int, int, int);
bool cercane(vector<vector<char>>, const string, int, int, int);
bool cercan(vector<vector<char>>, const string, int, int, int);
bool cercanw(vector<vector<char>>, const string, int, int, int);
bool trovata(vector<vector<char>>, vector<vector<vector<int>>>, const string, int, int, int);
void insertnode(node*&, node*&, decltype(paroladiz.size()), const string, vector<int>, vector<int>);
void sortlist(node*&);
void printlist(node*&, int, const string);


//funzione main: acquisisce tutti i dati necessari per l'esecuzione del programma

int main(){
    
    //inizializzazione variabili
    string percorso = "";
    string sequenza = "";
    string destinazione = "";
    
    //puntatori per la gestione e l'ordinamento della lista di risultati
    node* first = NULL;
    node* last = NULL;
    
    //numero parole da stampare
    int N = 0;
    
    // punteggio per ogni parola
    decltype(paroladiz.size()) punteggio = 0;
    
    //indici matrice 3D
    int x = 0; int y = 0; int z = 0;
    
    //titolo
    cout << "______________________|||WELCOME|||______________________\n"<<endl;
    
    //inserimento griglia
	cout << "Inserire N^2 caratteri e premere Invio, la matrice di gioco avrà dimensioni NxN, "
         << "con N numero naturale compreso tra 4 e 9 "<<endl;
    
    cin >> sequenza;
    
    unsigned long int S = sequenza.size();
    
    D = sqrt(S);
    
    while ((D != 4 && D != 5 && D != 6 && D != 7 && D != 8 && D != 9) || (!cin)) {
        cout << "Numero di caratteri inseriti non corretto!\n"<<endl;
        cout << "Inserire N^2 caratteri e premere Invio, la matrice di gioco avrà dimensioni NxN, "
        <<"con N numero naturale compreso tra 4 e 9"<<endl;
        
        sequenza = "";
        
        cin >> sequenza;
        
        unsigned long int S = sequenza.size();
        
        D = sqrt(S);
    }
    
    //creazione matrice dinamica di gioco
    mat.resize(D);
    for (int r = 0; r < D; ++r)
        mat[r].resize(D);
    
    //creazione matrice dinamica percorsi da scartare
    mat3D.resize(D);
    for (int r = 0; r < D; ++r) {
        mat3D[r].resize(D);
        
        for (int c = 0; c < D; ++c)
            mat3D[r][c].resize(8);
    }
    
    int idx = 0;
    while (idx < sequenza.size()) {
    for (i=0; i<D; i++){
        for (j=0; j<D; j++){
            mat[i][j]=sequenza[idx];
            idx++;
        }
    }
    }
    
    //visual griglia
	cout<<"Griglia di gioco:"<<endl;
    for (i=0; i<D; i++){
        for (j=0; j<D; j++){
            cout<<mat[i][j]<<" ";
        }
        cout<<endl;
    }
    
    //conversione eventuali caratteri inseriti maiuscoli in minuscoli
    for (i=0; i<D; i++){
        for (j=0; j<D; j++){
            mat[i][j] = tolower(mat[i][j]);
        }
    }
    
    //numero parole da stampare
    cout << "Inserire numero di parole da visualizzare: " <<endl;
    
    cin >> N;
    
    while (!cin || N <= 0) {       //se l'utente inserisce un numero negativo o qualcosa che non sia
                                   //un numero resetta lo stream di input
                                    
        cout << "Numero di parole da visualizzare non corretto, reinserire numero > 0: "<<endl;
        
        cin.clear();
        
        cin.ignore(1000000, '\n');
        
        cin >> N;
    }
    
    //percorso file dizionario
    cout << "Inserire percorso del file di dizionario, Invio per terminare: " <<endl;
    
    cin >> percorso;
    
    fstream diz;
    
    diz.open(percorso.c_str(), ios::in);
    
    if(!diz) {       //se il file non si trova nel percorso specificato
        
        cout << "File non trovato nel percorso indicato, riavviare il processo!" <<endl;
        
        return -1;
    }
    
    //percorso file di destinazione
    cout << "Inserire percorso del file di destinazione per i risultati, Invio per terminare: " <<endl;
    
    cin >> destinazione;
    
    cout << "Elaboro risultati...";
    
    //inizializzazione numero parole da stampare
    int index = 0;
    
    while (diz.good()) {            //finchè ci sono parole da leggere nel file
        
        x = 0;                      
        y = 0;
        
        getline(diz, paroladiz);    //acquisizione parola(IMPORTANTE: la parola viene acquisita con
                                    // i simboli "/r" alla fine della stringa
        
        //inizializzazione matrice 3D
        for (x = 0; x < D; x++) {
            for (y = 0; y < D; y++) {
                for (z = 0; z < 8; z++) {
                    
                    mat3D[x][y][z] = 0;
                }
            }
        }
        
        bool stessa = false;        //booleano per evitare di stampare doppioni
        
        if (paroladiz.size() > 2) {
            
            
            for (int i = 0; i < D; i++) {
                for (int j = 0; j < D; j++) {
                    
                    //verifico che la prima lettera della parola sia presente nella matrice
                    //altrimenti passo alla parola dopo
                    if (cercaprimo(mat, i, j) == true) {
                        
                        int pos = 1;                    //indice posizione all'interno della parola
                        
                        //funzione che cerca la parola all'interno della matrice,
                        //a sua volta ne chiama altre che effettuano il singolo confronto
                        //intorno alla lettera nella posizione in cui ci troviamo
                        if (trovata(mat, mat3D, paroladiz, i, j, pos) == true){
                            
                            //se non l'ho ancora stampata
                            if (stessa == false){
                                
                                //calcolo il punteggio
                                punteggio = punt(paroladiz, punteggio);
                                
                                //inserisco dati nella lista
                                insertnode(first, last, punteggio, paroladiz, righe, colonne);
                                
                                //aumento l'indice
                                index++;
                                
                                //parola già trovata, non viene stampata nuovamente se trovata
                                stessa = true;
                                
                            }
                            
                            
                        }
                    }
                    
                    //azzero matrice 3D
                    for (x = 0; x < D; x++) {
                        for (y = 0; y < D; y++) {
                            for (z = 0; z < 8; z++) {
                                
                                mat3D[x][y][z] = 0;
                            }
                        }
                    }
                    
                    //azzero vettori percorso
                    for (decltype(righe.size()) idx = 0; idx < righe.size(); idx++){
                        
                        righe.pop_back();
                        colonne.pop_back();
                    }
                    
                }
            }
        }
        
    }
    
    //ordino la lista dal punteggio più alto a quello più basso
    sortlist(first);
    
    //stampo i risultati su file
    printlist(first, N, destinazione);
    
    //operazione di visualizzazione del file tramite chiamata di sistema (incompatibile con Windows)
    string open = "open ";
    
    open = open + destinazione;
    
    system(open.c_str());
    
	return 0;
    
} //end main



//funzione che calcola i punteggi
decltype(paroladiz.size()) punt(const string parola, decltype(parola.size()) punti){
    
    //inizializzazione
    punti = 0;
    
    //calcolo dei punteggi in base alle lettere
    for (int idx = 0; idx < parola.size()-1; idx++) {
        
        if (parola[idx] == 'a' || 'b' ||  'c' || 'd' || 'e' || 'f' || 'g' || 'i' || 'j' || 'k' || 'l' || 'm' || 'n' || 'o' || 'p' || 'r' || 's' || 't' || 'v')
            
            punti = punti + 1;
        
        else if (parola[idx] == 'h' || 'q' || 'u' || 'z')
            
            punti = punti + 2;
        
        else if (parola[idx] == 'w' || 'x' || 'y')
            
            punti = punti + 3;
    }
    
    //moltiplico per lunghezza parola
    punti = punti * (parola.size()-1);
    
    //restituisco al chiamante il punteggio
    return punti;
    
}


    //funzione che verifica se stiamo andando a cercare in una posizione che è già presente nel percorso della parola in questione
bool presente(const vector<int> r, const vector<int> c, int i, int j){
    //inziz.
    bool usato = false;
    
    //confronto
    for (decltype(r.size()) idx = 0; idx < r.size(); idx++)
        if ((i == r[idx]) && (j == c[idx])) {
            usato = true;
        }
    
    //restituzione esito
    return usato;
}


    //funzione che verifica se la prima lettera della parola in questione è presente nella matrice
bool cercaprimo(vector<vector<char>> mat, int i, int j){
    //inziz.
    bool primo = false;
    
    //confronto
    if (paroladiz[0]==mat[i][j]){
        primo = true;
        
        //se la trova aggiunge posizione ai vettori del percorso
        righe.push_back(i);
        colonne.push_back(j);
    }
    
    //restituzione esito
    return primo;
}


    //**FUNZIONI CHE CERCANO LA LETTERA SUCCESSIVA DELLA PAROLA**

//ovest
bool cercaw(vector<vector<char>> mat, const string paroladiz, int i, int j, int p){
    
    //iniz.
    bool cerca = false;
    
    //cerco solo se non sono in posizioni che mi farebbero uscire dalla matrice (bordi)
    if (j > 0) {
        
        //se la posizione in cui sto cercando non è presente nei vettori del percorso
        if (presente(righe, colonne, i, j-1) == false){
            
            //confronto
            if (paroladiz[p]== mat[i][j-1]){
                cerca=true;
                
                //se è verificato, appendo posizione lettera trovata ai vettori del percorso 
                righe.push_back(i);
                colonne.push_back(j-1); }
        }
    }
    
    //restituzione
	return cerca;
}

//sud-ovest
bool cercasw(vector<vector<char>> mat, const string paroladiz, int i, int j, int p){
    
	bool cerca=false;
    if (j > 0 && i < D-1) {
        if (presente(righe, colonne, i+1, j-1) == false) {
            if (paroladiz[p]== mat[i+1][j-1]){
                cerca=true;
                righe.push_back(i+1);
                colonne.push_back(j-1); }
        }
    }
	return cerca;
}

//sud
bool cercas(vector<vector<char>> mat, const string paroladiz, int i, int j, int p){
    
	bool cerca=false;
    if (i < D-1) {
        if (presente(righe, colonne, i+1, j) == false) {
            if (paroladiz[p]== mat[i+1][j]){
                cerca=true;
                righe.push_back(i+1);
                colonne.push_back(j); }
        }
    }
	return cerca;
}

//sud-est
bool cercase(vector<vector<char>> mat, const string paroladiz, int i, int j, int p){
    
	bool cerca=false;
    if (i < D-1 && j < D-1) {
        if (presente(righe, colonne, i+1, j+1) == false) {
            if (paroladiz[p]== mat[i+1][j+1]){
                cerca=true;
                righe.push_back(i+1);
                colonne.push_back(j+1); }
        }
    }
	return cerca;
}

//est
bool cercae(vector<vector<char>> mat, const string paroladiz, int i, int j, int p){
    
	bool cerca=false;
    if (j < D-1) {
        if (presente(righe, colonne, i, j+1) == false) {
            if (paroladiz[p]== mat[i][j+1]){
                cerca=true;
                righe.push_back(i);
                colonne.push_back(j+1); }
        }
    }
	return cerca;
}

//nord-est
bool cercane(vector<vector<char>> mat, const string paroladiz, int i, int j, int p){
    
	bool cerca=false;
    if (i > 0 && j < D-1) {
        if (presente(righe, colonne, i-1, j+1) == false) {
            if (paroladiz[p]== mat[i-1][j+1]){
                cerca=true;
                righe.push_back(i-1);
                colonne.push_back(j+1); }
        }
    }
	return cerca;
}

//nord
bool cercan(vector<vector<char>> mat, const string paroladiz, int i, int j, int p){
    
	bool cerca=false;
    if (i > 0) {
        if (presente(righe, colonne, i-1, j) == false) {
            if (paroladiz[p]== mat[i-1][j]){
                cerca=true;
                righe.push_back(i-1);
                colonne.push_back(j); }
        }
    }
	return cerca;
}

//nord-ovest
bool cercanw(vector<vector<char>> mat, const string paroladiz, int i, int j, int p){
    
	bool cerca=false;
    if (i > 0 && j > 0) {
        if (presente(righe, colonne, i-1, j-1) == false) {
            if (paroladiz[p]== mat[i-1][j-1]){
                cerca=true;
                righe.push_back(i-1);
                colonne.push_back(j-1); }
        }
    }
	return cerca;
}





//funzione che cerca la parola all'interno della matrice, a sua volta chiama le altre sopra.
//Definita ricorsivamente, ogni volta che trova la lettere successiva della parola, si richiama
//aggiornando la posizione di partenza della ricerca, l'indice della posizione all'interno della
//parola e i vettori del percorso(aggiornati dalle singole funzioni di ricerca)
bool trovata(vector<vector<char>> mat, vector<vector<vector<int>>> mat3D, const string paroladiz, int i, int j, int pos){
    
    //finchè non sono arrivato a fine parola
    while (pos < paroladiz.size() - 1) {
        
        //**SCANDISCE TUTTI I PERCORSI POSSIBILI**
        
        //se dove sto cercando non sono ancora andato e quindi non so se è un vicolo ceco o no
        if (mat3D[i][j][0] == 0){
            
            //se la ricerca a ovest ha restituito true 
            if (cercaw(mat, paroladiz, i, j, pos) == true) {
                
                //richiamo la funzione aggiornando i parametri specificati sopra
                return trovata(mat, mat3D, paroladiz, i, j-1, ++pos);
            }
            else {
                
                //altrimenti se la ricerca ha restituito false perchè in quella posizione siamo già passati
                if (presente(righe, colonne, i, j-1) == true) {
                    
                    //vuol dire che lì non devo più cercare
                    mat3D[i][j][0] = 1;
                    
                    //richiamo la funzione con gli stessi parametri, questa volta non cercherà più
                    //dove siamo già passati
                    return trovata(mat, mat3D, paroladiz, i, j, pos);
                    
                }
                
            }
        }
        
        //sud-ovest
        if (mat3D[i][j][1] == 0){
            if (cercasw(mat, paroladiz, i, j, pos) == true) {
                
                return trovata(mat, mat3D, paroladiz, i+1, j-1, ++pos);
            }
            else {
                if (presente(righe, colonne, i+1, j-1) == true) {
                    
                    mat3D[i][j][1] = 1;
                    
                    return trovata(mat, mat3D, paroladiz, i, j, pos);
                    
                }
                
            }
        }
        
        //sud
        if (mat3D[i][j][2] == 0){
            if (cercas(mat, paroladiz, i, j, pos) == true) {
                
                return trovata(mat, mat3D, paroladiz, i+1, j, ++pos);
            }
            else {
                if (presente(righe, colonne, i+1, j) == true) {
                    
                    mat3D[i][j][2] = 1;
                    
                    return trovata(mat, mat3D, paroladiz, i, j, pos);
                    
                }
                
            }
        }
        
        //sud-est
        if (mat3D[i][j][3] == 0){
            if (cercase(mat, paroladiz, i, j, pos) == true) {
                
                return trovata(mat, mat3D, paroladiz, i+1, j+1, ++pos);
            }
            else {
                if (presente(righe, colonne, i+1, j+1) == true) {
                    
                    mat3D[i][j][3] = 1;
                    
                    return trovata(mat, mat3D, paroladiz, i, j, pos);
                    
                }
                
            }
        }
        
        //est
        if (mat3D[i][j][4] == 0){
            if (cercae(mat, paroladiz, i, j, pos) == true) {
                
                return trovata(mat, mat3D, paroladiz, i, j+1, ++pos);
            }
            else {
                if (presente(righe, colonne, i, j+1) == true) {
                    
                    mat3D[i][j][4] = 1;
                    
                    return trovata(mat, mat3D, paroladiz, i, j, pos);
                    
                }
                
            }
        }
        
        //nord-est
        if (mat3D[i][j][5] == 0){
            if (cercane(mat, paroladiz, i, j, pos) == true) {
                
                return trovata(mat, mat3D, paroladiz, i-1, j+1, ++pos);
            }
            else {
                if (presente(righe, colonne, i-1, j+1) == true) {
                    
                    mat3D[i][j][5] = 1;
                    
                    return trovata(mat, mat3D, paroladiz, i, j, pos);
                    
                }
                
            }
        }
        
        //nord
        if (mat3D[i][j][6] == 0){
            if (cercan(mat, paroladiz, i, j, pos) == true) {
                
                return trovata(mat, mat3D, paroladiz, i-1, j, ++pos);
            }
            else {
                if (presente(righe, colonne, i-1, j) == true) {
                    
                    mat3D[i][j][6] = 1;
                    
                    return trovata(mat, mat3D, paroladiz, i, j, pos);
                    
                }
                
            }
        }
        
        //nord-ovest
        if (mat3D[i][j][7] == 0 && cercanw(mat, paroladiz, i, j, pos) == true){
            
            return trovata(mat, mat3D, paroladiz, i-1, j-1, ++pos); }
        
        
        else {                          //se da quella posizione intorno non si trova la lettera
                                        //successiva indietreggio di una posizione
                                        //andando a vedere il percorso della parola al punto precedente
            
            //se sono alla prima lettera della parola vuol dire che la parola non è presente nella matrice
            if (pos == 1) {
                
                righe.pop_back();
                colonne.pop_back();
                
                return false;
            }
            
            //altrimenti
            else {
                
                //iniz.
                int h = 0;
                int k = 0;
                
                //DEVO CAPIRE DA CHE POSIZIONE SONO ARRIVATO, QUINDI EFFETTUO UN CONFRONTO CON LE
                //PENULTIME COORDINATE MEMORIZZATE NEI VETTORI DEL PERCORSO
                
                h = righe[righe.size()-2];
                k = colonne[colonne.size()-2];
                
                //controllo sempre di poter effettuare i confronti senza uscire dalla matrice
                if (j > 0) {
                    
                    //se c'è corrispondenza tra le coordinate a ovest e la penultima posizione
                    //memorizzata nei vettori del percorso
                    if ((h == i) && (k == j-1)) {
                        
                        //vuol dire che prima sono arrivato da est, quindi al prossimo giro non
                        //cercherò più in quella direzione dal momento che mi porta in un vicolo cieco
                        mat3D[h][k][4] = 1;
                        
                        //resetto i percorsi possibili partendo dalla penultima posizione
                        for (int s = 0; s < 8; s++) {
                            mat3D[i][j][s] = 0;
                        }
                        
                        //elimino la posizione corrente dal percorso (dove mi trovo ora è un vicolo
                        //cieco
                        
                        righe.pop_back();
                        colonne.pop_back();
                        
                    } }
                
                //**EFFETTUO LO STESSO PROCEDIMENTO SE SONO ARRIVATO DA UNA QUALSIASI ALTRA
                //DIREZIONE**
                
                if ((i < D) && (j > 0)) {
                    
                    if ((h == i+1) && (k == j-1)) {
                        
                        mat3D[h][k][5] = 1;
                        
                        for (int s = 0; s < 8; s++) {
                            mat3D[i][j][s] = 0;
                        }
                        
                        righe.pop_back();
                        colonne.pop_back();
                        
                    } }
                
                if (i < D) {
                    
                    if ((h == i+1) && (k == j)) {
                        
                        mat3D[h][k][6] = 1;
                        
                        for (int s = 0; s < 8; s++) {
                            mat3D[i][j][s] = 0;
                        }
                        
                        righe.pop_back();
                        colonne.pop_back();
                        
                    } }
                
                if (i < D && j < D) {
                    
                    if ((h == i+1 && k == j+1)) {
                        
                        mat3D[h][k][7] = 1;
                        
                        for (int s = 0; s < 8; s++) {
                            mat3D[i][j][s] = 0;
                        }
                        
                        righe.pop_back();
                        colonne.pop_back();
                        
                    } }
                
                if (j < D) {
                    
                    if ((h == i) && (k == j+1)) {
                        
                        mat3D[h][k][0] = 1;
                        
                        for (int s = 0; s < 8; s++) {
                            mat3D[i][j][s] = 0;
                        }
                        
                        righe.pop_back();
                        colonne.pop_back();
                        
                    } }
                
                if (i > 0 && j < D) {
                    
                    if ((h == i-1) && (k == j+1)) {
                        
                        mat3D[h][k][1] = 1;
                        
                        for (int s = 0; s < 8; s++) {
                            mat3D[i][j][s] = 0;
                        }
                        
                        righe.pop_back();
                        colonne.pop_back();
                        
                    } }
                
                if (i > 0) {
                    
                    if ((h == i-1) && (k == j)) {
                        
                        mat3D[h][k][2] = 1;
                        
                        for (int s = 0; s < 8; s++) {
                            mat3D[i][j][s] = 0;
                        }
                        
                        righe.pop_back();
                        colonne.pop_back();
                        
                    } }
                
                if (i > 0 && j > 0) {
                    
                    if ((h == i-1) && (k == j-1)) {
                        
                        mat3D[h][k][3] = 1;
                        
                        for (int s = 0; s < 8; s++) {
                            mat3D[i][j][s] = 0;
                        }
                        
                        righe.pop_back();
                        colonne.pop_back();
                        
                    } }
                
                //Richiamo la funzione con i parametri aggiornati, non cercherà dove ho stabilito
                //con i passaggi precedenti
                return trovata(mat, mat3D, paroladiz, i = righe[righe.size()-1], j = colonne[colonne.size()-1], --pos); }
        }
    }
    
    //se sono a fine parola vuol dire che l'ho trovata, restituisco true
    if (pos == paroladiz.size() - 1 )
        return true;
}




//funzione che inserisce man mano che le parole vengono trovate, i blocchi all'interno della lista
//dei risultati(inserimento in coda "intelligente", più veloce, le parole con stesso punteggio
//rimangono ordinate in ordine alfabetico
void insertnode(node*& first, node*& last, decltype(paroladiz.size()) punteggio, const string paroladiz, vector<int> righe, vector<int> colonne){
    
    node* newnode = new node;
    
    newnode->parola = paroladiz;
    
    newnode->punti = punteggio;
    
    newnode->r = righe;
    
    newnode->c = colonne;
    
    newnode->nextptr = NULL;
    
    
    //se la lista è vuota
    
    if (first == NULL) {
        
        first = newnode;
        last = newnode;
        
    }
    
    //altrimenti aggancio
    
    else {
        
        last->nextptr = newnode;
        last = newnode;
    }
    
}

//funzione di ordinamento lista tramite bubble sort. L'algoritmo esegue confronti nidificati a due a due, se il primo valore è minore del secondo li scambia e così via fino a che la lista non è ordinata
void sortlist(node*& head)
{
    node *a = NULL;
    node *b = NULL;
    node *c = NULL;
    node *d = NULL;
    node *scambio = NULL;
    
    while(d != head->nextptr)
    {
        c = a = head;
        b = head->nextptr;
        
        while(a!=d)
        {
            if(a->punti < b->punti)
            {
                if(a==head)
                {
                    scambio = b->nextptr;       //scambio elementi
                    b->nextptr = a;
                    a->nextptr = scambio;
                    head = b;
                    c = b;
                }
                
                else
                {
                    scambio = b->nextptr;       //scambio elementi
                    b->nextptr = a;
                    a->nextptr = scambio;
                    c->nextptr = b;
                    c = b;
                }
            }
            
            else
            {
                c = a;
                a = a->nextptr;
            }
            
            b = a->nextptr;
            
            if(b == d)
                d = a;
        }
    }
}


//stampa la lista su file specificato
void printlist(node*& head, int N, const string dest){
    
    int counter = 0;
    
    node*currentnode = head;
    
    //creazione e apertura del file(o sovrascrittura se già esistente)
    ofstream ris(dest);
    if(!ris) {
        cout << "Errore nella creazione del file!";
        system("PAUSE");
    }
    
    ris << "Risultati: " <<endl;
    
    //scorro tutta la lista a partire dalla testa e ogni volta stampo i contenuti di ogni blocco
    while (currentnode!=NULL && counter < N) {
        
        ris << currentnode->parola << " - " << currentnode->punti << " ";
        
        for (int i = 0; i < currentnode->r.size(); i++) {
            
            ris << "[" << currentnode->r[i] << "][" << currentnode->c[i] << "]; ";
        }
        
        ris <<endl;
        
        currentnode = currentnode->nextptr;
        
        counter++;
        
    }
    
    ris.close();
    
}



