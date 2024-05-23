# README

## Descriere generală

Această temă reprezintă un client scris în limbajul C care interacționează cu un server pentru a realiza operațiuni precum înregistrare, autentificare, accesarea unei biblioteci, și gestionarea cărților. Clientul trimite cereri HTTP către server și procesează răspunsurile primite.

## Funcționalități

Clientul poate efectua următoarele acțiuni:

1. **Înregistrare utilizator** (`register`)
2. **Autentificare utilizator** (`login`)
3. **Accesare bibliotecă** (`enter_library`)
4. **Obținerea listei de cărți** (`get_books`)
5. **Obținerea detaliilor unei cărți** (`get_book`)
6. **Adăugarea unei cărți noi** (`add_book`)
7. **Ștergerea unei cărți** (`delete_book`)
8. **Deconectare utilizator** (`logout`)
9. **Ieșire din program** (`exit`)

## Cum funcționează codul

### Structura Codului

Codul este structurat pentru a realiza diverse operațiuni prin intermediul unui meniu interactiv în linia de comandă. Utilizatorul poate introduce comenzi care corespund operațiunilor menționate mai sus.

### Biblioteci utilizate

- `stdio.h`, `stdlib.h`, `string.h`: Funcții standard de I/O și gestionare a memoriei.
- `unistd.h`, `sys/socket.h`, `netinet/in.h`, `netdb.h`, `arpa/inet.h`: Funcții pentru gestionarea conexiunilor de rețea.
- `parson.h`: O bibliotecă pentru manipularea JSON ,menționată în enunțul temei.
- `helpers.h`, `requests.h`, `buffer.h`: Biblioteci definite de utilizator pentru ajutor în construirea și trimiterea cererilor HTTP.

### Descrierea funcționalităților

#### Înregistrare (`register`)

- Verifică dacă utilizatorul este deja autentificat.
- Colectează datele utilizatorului (username și password).
- Creează un obiect JSON cu aceste date și trimite o cerere POST la server.
- Procesează răspunsul serverului pentru a afișa un mesaj de succes sau eroare.

#### Autentificare (`login`)

- Verifică dacă utilizatorul este deja autentificat.
- Colectează datele utilizatorului (username și password).
- Creează un obiect JSON cu aceste date și trimite o cerere POST la server.
- Procesează răspunsul serverului și extrage cookie-ul de autentificare dacă autentificarea este reușită.

#### Accesare bibliotecă (`enter_library`)

- Verifică dacă utilizatorul este autentificat.
- Trimite o cerere GET pentru a accesa biblioteca.
- Procesează răspunsul și extrage token-ul de acces.

#### Obținerea listei de cărți (`get_books`)

- Verifică dacă utilizatorul este autentificat.
- Trimite o cerere GET pentru a obține lista de cărți disponibile în bibliotecă.
- Procesează și afișează răspunsul.

#### Obținerea detaliilor unei cărți (`get_book`)

- Verifică dacă utilizatorul este autentificat.
- Solicită ID-ul cărții.
- Trimite o cerere GET pentru a obține detaliile cărții specificate.
- Procesează și afișează răspunsul.

#### Adăugarea unei cărți noi (`add_book`)

- Verifică dacă utilizatorul este autentificat.
- Colectează detaliile cărții (titlu, autor, gen, publisher, număr de pagini).
- Creează un obiect JSON cu aceste detalii și trimite o cerere POST la server.
- Procesează răspunsul și afișează un mesaj de succes sau eroare.

#### Ștergerea unei cărți (`delete_book`)

- Verifică dacă utilizatorul este autentificat.
- Solicită ID-ul cărții care trebuie ștearsă.
- Trimite o cerere DELETE pentru a șterge cartea specificată.
- Procesează răspunsul și afișează un mesaj de succes sau eroare.

#### Deconectare (`logout`)

- Verifică dacă utilizatorul este autentificat.
- Trimite o cerere GET pentru a efectua deconectarea.
- Procesează răspunsul și resetează starea de autentificare.

#### Ieșire (`exit`)

- Închide conexiunea la server și eliberează memoria alocată.

### Rularea programului

Pentru a rula programul, trebuie să compilați sursa utilizând un compilator C (de exemplu, gcc):

```bash
gcc -o client client.c helpers.c requests.c buffer.c parson.c
```

După compilare, puteți rula programul cu comanda:

```bash
./client
```

## Autor

- **Artiom Pujleacov** 