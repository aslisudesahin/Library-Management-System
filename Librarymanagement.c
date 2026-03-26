
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h> 
#include <ctype.h>  

#define MAX_BOOKS 500
#define MAX_USERS 100
#define MAX_BORROWED_PER_USER 3 

#define FILENAME_BOOKS "kitaplar.txt"
#define FILENAME_USERS "kullanicilar.txt"
#define FILENAME_BORROWED "oduncKitaplar.txt"
typedef struct { 
    char kitapID[10];  
    char kitapAdi[100];
    char yazarAdi[100];
    char kategori[50];    
    bool oduncAlindiMi; // kitapların ödünç durumu kontrol eder true-false ile.
    int toplamAdet; //toplam kitap adedini gösterir.
    int mevcutAdet; //ödünç alınabilecek kitap adeini gösterir.      
} Kitap;

typedef struct {
    char kullaniciAd[50];
    char kullaniciSifre[50];
    char rol[10]; // "admin", "ogrenci", "personel"
} Kullanici;

typedef struct {
    int gun, ay, yil;//kitap alım ve iade tarihi için oluşturuldu.
} tarih;

typedef struct {
    char oduncID[10]; 
    char kullaniciAdi[50]; 
    char kitapID[10];      
    tarih oduncTarihi;
    tarih iadeTarihi;
} OduncKitap;


Kitap kitaplar[MAX_BOOKS];
Kullanici kullanicilar[MAX_USERS];
OduncKitap odunckitaplar[MAX_BOOKS]; 

int bookCount = 0;//kitap sayısı sayacı.
int userCount = 0;//kullanıcı sayacı.
int borrowedCount = 0;//ödünç kitap sayacı.
int girisYapanKullaniciIndex = -1; //giriş yapan kullanıcıların sayaci, ad-şifrelerini doğrulama amacıyla oluşturuldu.
//Yardımcı fonksiyonlar:
int findBookIndexById(const char *id);
int findBookIndexByName(const char *name);
tarih getCurrentDate();
tarih addDaysToDate(tarih date, int days);
int getDaysDifference(tarih d1, tarih d2);
bool isDateGreaterThan(tarih d1, tarih d2);
void generateBookID(char *idBuffer);
void generateBorrowID(char *idBuffer);
bool kullaniciDogrulama(const char *kullaniciadi, const char *sifre, const char *rol);

void loginMenu();
void adminMenu();
void userMenu();
void addBooks();
void loadBooks();
void saveBooks();
void addUsers();
void loadUsers();
void saveUsers();
void updateBook();
void deleteBook();
void searchBooks(); 
void checkOverdueBooks();
void listAllBorrowedBooks();
void borrowBook();
void returnBook();
void viewBorrowedBooks();
void loadBorrowedBooks(); 
void saveBorrowedBooks(); 

int findBookIndexById(const char *id) { //kitap aramalarında ID'ye göre arama yapar.
    for (int i = 0; i < bookCount; i++) {
        if (strcmp(kitaplar[i].kitapID, id) == 0) {
            return i;
        }
    }
    return -1; 
}
int findBookIndexByName(const char *name) { //kitap aramalarında isme göre arama yapar.
    for (int i = 0; i < bookCount; i++) {
        if (strcmp(kitaplar[i].kitapAdi, name) == 0) {
            return i;
        }
    }
    return -1; 
}
void generateBookID(char *idBuffer) { //kitaplara belirli sırayla belirli bir ID verir.
    sprintf(idBuffer, "B%03d", bookCount + 1); 
}
void generateBorrowID(char *idBuffer) { //ödünç alınan kitaplara belirli sırayla belirli bir ID verir.
    sprintf(idBuffer, "BR%03d", borrowedCount + 1); 
}
tarih getCurrentDate() { //bilgisayardan güncel tarih alır.
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    tarih bugun;
    bugun.gun = tm.tm_mday;
    bugun.ay = tm.tm_mon + 1; 
    bugun.yil = tm.tm_year + 1900; 
    return bugun;
}
bool isDateGreaterThan(tarih d1, tarih d2) {// gecikme olup olmadıgını kontrol etmek için tarihleri karşılastırır.
    if (d1.yil > d2.yil) return true;
    if (d1.yil < d2.yil) return false;
    // yıllar eşit, aylar kontrol edilir
    if (d1.ay > d2.ay) return true;
    if (d1.ay < d2.ay) return false;
    // aylar eşit, günler kontrol edilir
    return d1.gun > d2.gun;
}
tarih addDaysToDate(tarih date, int days) { //kitabın odunc alındıgı tarıhe odunc suresının eklenmesı ıcın olusturuldu.
                                          
    date.gun += days;
    if (date.gun > 30) { //odunc suresı eklendıgınde gun 30 u gecıyorsa 30 gun ay kısmına +1 ay olarak eklenıyor.
        date.gun -= 30;
        date.ay++;
        if (date.ay > 12) { ////odunc suresı eklendıgınde ay 12 yı gecıyorsa 12 ay yıl kısmına +1 yıl olarak eklenıyor
            date.ay -= 12;
            date.yil++;
        }
    }
    return date;
}
int getDaysDifference(tarih d1, tarih d2) {//tarih farkını gün olarak alır.
    int days1 = d1.yil * 360 + d1.ay * 30 + d1.gun;
    int days2 = d2.yil * 360 + d2.ay * 30 + d2.gun;
    return abs(days1 - days2);
}

bool kullaniciDogrulama(const char *kullaniciadi, const char *sifre, const char *rol) {// kaydedilen kullaniciyi doğrular.
    for (int i = 0; i < userCount; i++) {
        if (strcmp(kullanicilar[i].kullaniciAd, kullaniciadi) == 0 &&
            strcmp(kullanicilar[i].kullaniciSifre, sifre) == 0 &&
            strcmp(kullanicilar[i].rol, rol) == 0) {
            girisYapanKullaniciIndex = i; 
            return true;
        }
    }
    return false;
}

int main() {
    printf("Kutuphane Yonetim Sistemine Hos Geldiniz!\n");

    loadBooks();
    loadUsers();
    loadBorrowedBooks();

    loginMenu();

    saveBooks();
    saveUsers();
    saveBorrowedBooks();

    printf("Sistemden cikiliyor. Gule Gule!");
    return 0;
}

void loginMenu() { //giriş menusu.
    char ad[30];
    char sifre[30];
    int secim;

    while (true) {
        printf("\nGiris Menusu\n");
        printf("1. Yonetici Girisi\n");
        printf("2. Kullanici Girisi\n");
        printf("3. Cikis\n");
        printf("Seciminizi yapin: ");
        if (scanf("%d", &secim) != 1) { //kullnıcının gecerli girişi kontrol edilir,
            printf("Gecersiz giris. Lutfen bir sayi girin.");
            while (getchar() != '\n');// girilen gecersiz girdiler temizlenir. 
            continue;
        }
        getchar(); 

        switch (secim) {
            case 1: // Admin giris
                printf("Yonetici Kullanici Adi: ");
                fgets(ad, sizeof(ad), stdin);
                ad[strcspn(ad, "\n")] = 0;
                printf("Yonetici Sifre: ");
                fgets(sifre, sizeof(sifre), stdin);
                sifre[strcspn(sifre, "\n")] = 0; 
                
                if (kullaniciDogrulama(ad, sifre, "admin")) {
                    printf("Yonetici girisi basarili!");
                    adminMenu(); 
                    return;
                }
                else  {
                    printf("Hatali kullanici adi veya sifre. Tekrar deneyin.");
                }
                break;
            case 2: // kullanici giris
                printf("Kullanici Adi: ");
                fgets(ad, sizeof(ad), stdin);
                ad[strcspn(ad, "\n")] = 0; 
                printf("Sifre: ");
                fgets(sifre, sizeof(sifre), stdin);
                sifre[strcspn(sifre, "\n")] = 0; 
                
                if (kullaniciDogrulama(ad,sifre, "ogrenci") || kullaniciDogrulama(ad,sifre, "personel")) { 
                    printf("Kullanici girisi basarili!");
                    userMenu();
                    return; 
                } else {
                    printf("Hatali kullanici adi veya sifre. Tekrar deneyin.");
                }
                break;
            case 3: 
                return; 
            default:
                printf("Gecersiz secim. Lutfen tekrar deneyin.");
                break;
         }  
    }   
}
void adminMenu() { // admin menusu
    int secim;
    do {
        printf("\nYonetici Menusu\n");
        printf("1. Kitap Ekle\n");
        printf("2. Kitap Guncelle\n");
        printf("3. Kitap Sil\n");
        printf("4. Kullanici Ekle\n");
        printf("5. Kitap Ara\n");
        printf("6. Tum Odunc Alinan Kitaplari Listele\n");
        printf("7. Geciken Kitaplari Kontrol Et\n"); 
        printf("8. Cikis\n");
        printf("Seciminizi girin: ");
        if (scanf("%d", &secim) != 1) {//kullnıcının gecerli girişi kontrol edilir.
            printf("Gecersiz giris. Lutfen bir sayi girin.");
            while (getchar() != '\n');
            continue;
        }
        getchar(); 

        switch (secim) { //adminin ulaşabildiği özellikler
            case 1: addBooks(); break;
            case 2: updateBook(); break;
            case 3: deleteBook(); break;
            case 4: addUsers(); break;
            case 5: searchBooks(); break;
            case 6: listAllBorrowedBooks(); break; 
            case 7: checkOverdueBooks(); break;
            case 8: printf("Ana menuye donuluyor..."); break;
            default: printf("Gecersiz secim! Lutfen tekrar deneyin.");
        }
    } while (secim != 8);
}
void userMenu() { //kullanici menusü
    int secim;
    do {
        printf("\nKullanici Menusu\n");
        printf("1. Kitap Ara\n");
        printf("2. Kitap Odunc Al\n");
        printf("3. Kitap Iade Et\n");
        printf("4. Aldigim Kitaplari Goruntule\n");
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        if (scanf("%d", &secim) != 1) {//kullnıcının gecerli girişi kontrol edilir.
            printf("Gecersiz giris. Lutfen bir sayi girin.");
            while (getchar() != '\n');
            continue;
        }
        getchar(); 

        switch (secim) {// kullanıcının erisebildiği özellikler.  
            case 1: searchBooks(); break;
            case 2: borrowBook(); break;
            case 3: returnBook(); break;
            case 4: viewBorrowedBooks(); break;
            case 0: printf("Cikis yapiliyor..."); break;
            default: printf("Gecersiz secim. Tekrar deneyin.");
        }
    } while (secim != 0);
}

void addBooks() { // kitap ekleme .
    if (bookCount >= MAX_BOOKS) {
        printf("Kitap sayisi maksimuma ulasti!");
        return;
    }

    Kitap yeni;
    generateBookID(yeni.kitapID); 

    printf("Kitap Adi: ");
    fgets(yeni.kitapAdi, sizeof(yeni.kitapAdi), stdin);
    yeni.kitapAdi[strcspn(yeni.kitapAdi, "\n")] = 0; // girilen girdinin  düzgün okunması için konuldu. (fgets fazladan boşluk ekliyor buyuzden dizi düzgün yazılamıyor bosluk strcspn ile siliniyor.)

    printf("Yazar Adi: ");
    fgets(yeni.yazarAdi, sizeof(yeni.yazarAdi), stdin);
    yeni.yazarAdi[strcspn(yeni.yazarAdi, "\n")] = 0;

    printf("Kategori: ");
    fgets(yeni.kategori, sizeof(yeni.kategori), stdin);
    yeni.kategori[strcspn(yeni.kategori, "\n")] = 0;

    printf("Toplam Adet: "); 
    if (scanf("%d", &yeni.toplamAdet) != 1 || yeni.toplamAdet <= 0)  {// ilk sorgulama sayi girilip girilmediğini, 2.sorgulama pozitif sayı girilip girilmediğini konrtol ediyor.
        printf("Gecersiz adet. Kitap ekleme iptal edildi.");
        while (getchar() != '\n'); //girilen geçersiz işlemler temizlenir.
        return;
    }
    getchar(); 

    yeni.mevcutAdet = yeni.toplamAdet; 
    yeni.oduncAlindiMi = false; //kitap yeni ekelndi henuz odünç alınmadıgı ıcın false dönüyor.

    kitaplar[bookCount++] = yeni;
    saveBooks();

    printf("Kitap basariyla eklendi. ID: %s\n", yeni.kitapID);
}

void loadBooks() { //kitapların dosyalanması.
    FILE *file = fopen(FILENAME_BOOKS, "r");
    if (file == NULL) {
        printf("Kitaplar dosyasi bulunamadi.");
        return;
    }

    char tmpnsatir[500]; // Satırı tutacak tampon (program kapatılıp açıldığında önceden eklenen kitapların kalması için tampon konuldu. )
    bookCount = 0;

    while (fgets(tmpnsatir, sizeof(tmpnsatir), file) != NULL && bookCount < MAX_BOOKS) {
        // Satırdan verileri ayrıştırır sscanf.
        if (sscanf(tmpnsatir, "%s\t%[^\t]\t%[^\t]\t%[^\t]\t%d\t%d\t%d",
                   kitaplar[bookCount].kitapID,
                   kitaplar[bookCount].kitapAdi,
                   kitaplar[bookCount].yazarAdi,
                   kitaplar[bookCount].kategori,
                   (int *)&kitaplar[bookCount].oduncAlindiMi,//oduncAlindiMi bool türündedir. Dosyalama sisteminde int olarak girimesi gerektği için (int*) ifadesi kullanıldı.
                   &kitaplar[bookCount].toplamAdet,
                   &kitaplar[bookCount].mevcutAdet) == 7) {
            bookCount++;
        }
    }

    fclose(file);
    printf("Kitap verileri yuklendi.\n");
}

void saveBooks() { //dosyalanan kitapların saklanması.
    if (bookCount == 0) {
        printf("Uyari: Kaydedilecek kitap yok. Dosya sifirlanmayacak.\n");
        return;
    }

    FILE *file = fopen(FILENAME_BOOKS, "w");
    if (file == NULL) {
        printf("Kitaplar dosyasi yazilamadi");
        return;
    }

    for (int i = 0; i < bookCount; i++) {
        fprintf(file, "%s\t%s\t%s\t%s\t%d\t%d\t%d\n",
                kitaplar[i].kitapID,
                kitaplar[i].kitapAdi,
                kitaplar[i].yazarAdi,
                kitaplar[i].kategori,
                (int)kitaplar[i].oduncAlindiMi,
                kitaplar[i].toplamAdet,
                kitaplar[i].mevcutAdet);
    }

    fclose(file);
    printf("Kitap verileri kaydedildi.\n");
}
void updateBook() { //kitapların güncellenmesi.
    char id[10];
    printf("Guncellenecek kitabin ID'sini girin: ");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = 0; // girilen girdinin  düzgün okunması için konuldu. (fgets fazladan boşluk ekliyor buyuzden dizi düzgün yazılamıyor bosluk strcspn ile siliniyor.)

    int index = findBookIndexById(id);
    if (index == -1) {
        printf("Kitap bulunamadi.");
        return;
    }

    Kitap *k = &kitaplar[index]; //daha kolay ulaşım amacıyla pointer kullanıldı.
    char girdi[100];  //güncellenen kitaplar ilk önce burada tutulacak.

    printf("Yeni Kitap Adi (degistirmek istemiyorsaniz bos birakin): ");
    fgets(girdi, sizeof(girdi), stdin);
    girdi[strcspn(girdi, "\n")] = 0;
    if (strlen(girdi) > 0) {    //boş bırakılıp bırakılmadığı konrol ediliyor.
        strcpy((*k).kitapAdi, girdi);  // boş bırakılmadıysa  girdi dizisi kitaplara kopyalanıyor.
    }

    printf("Yeni Yazar Adi (degistirmek istemiyorsaniz bos birakin): ");
    fgets(girdi, sizeof(girdi), stdin);
    girdi[strcspn(girdi, "\n")] = 0;
    if (strlen(girdi) > 0) {
        strcpy((*k).yazarAdi, girdi);
    }

    printf("Yeni Kategori (degistirmek istemiyorsaniz bos birakin): ");
    fgets(girdi, sizeof(girdi), stdin);
    girdi[strcspn(girdi, "\n")] = 0;
    if (strlen(girdi) > 0) {
        strcpy((*k).kategori, girdi);
    }
    saveBooks();
    printf("Kitap bilgileri guncellendi.");
}
void deleteBook() {// kitap silme
    char id[10];
    printf("Silinecek kitabin ID'sini girin: ");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = 0;

    int index = findBookIndexById(id);
    if (index == -1) {
        printf("Kitap bulunamadi.");
        return;
    }

    
    for (int i = 0; i < borrowedCount; i++) { // odunc alınma durumu kontrol ediliyor.
        if (strcmp(odunckitaplar[i].kitapID, kitaplar[index].kitapID) == 0) {
            printf("Bu kitap su anda odunc alinmis. Silinemez.");
            return;
        }
    }

    for (int i = index; i < bookCount - 1; i++) { // silinecek olan kitap baslangıç olarak alınır. döngü sondan bir öncekine kadar devam eder.
        kitaplar[i] = kitaplar[i + 1];  //dizi elemaları bir öncekinin yerine geçer.
    }
    bookCount--;

    saveBooks();
    printf("Kitap basariyla silindi.");
}
void addUsers() {// kullanıcı ekleme.
    if (userCount >= MAX_USERS) {
        printf("Maksimum kullanici sayisina ulasildi.");
        return;
    }

    Kullanici yeni;
    printf("Kullanici adini giriniz: ");
    fgets(yeni.kullaniciAd, sizeof(yeni.kullaniciAd), stdin);
    yeni.kullaniciAd[strcspn(yeni.kullaniciAd, "\n")] = 0;
    
    printf("Kullanici sifresini giriniz: ");
    fgets(yeni.kullaniciSifre, sizeof(yeni.kullaniciSifre), stdin);
    yeni.kullaniciSifre[strcspn(yeni.kullaniciSifre, "\n")] = 0;

    int rolSecim;
    printf("Kullanici rolunu giriniz (1: Ogrenci, 2: Personel): ");
    if (scanf("%d", &rolSecim) != 1) {  //geçerli giriş durumu komtrol ediliyor.
        printf("Gecersiz giris. Kullanici ekleme iptal edildi.");
        while (getchar() != '\n'); 
        return;
    }
    getchar(); 

    if (rolSecim == 1) { 
        strcpy(yeni.rol, "ogrenci");
    } else if (rolSecim == 2) {
        strcpy(yeni.rol, "personel");
    } else {
        printf("Gecersiz rol secimi. Kullanici eklenemedi.");
        return;
    }

    kullanicilar[userCount] = yeni;
    userCount++;
    saveUsers(); 
    printf("Kullanici basariyla eklendi.");
}
void loadUsers() {// kullanıcı dosyalama
    FILE *f = fopen(FILENAME_USERS, "r");
    if (f == NULL) {
        printf("Kullanici dosyasi bulunamadi. Varsayilan admin olusturulacak.");
        if (userCount == 0) { //varsayılan admin oluşturuluyor.
            strcpy(kullanicilar[0].kullaniciAd, "admin");
            strcpy(kullanicilar[0].kullaniciSifre, "admin1234");
            strcpy(kullanicilar[0].rol, "admin");
            userCount++;
            saveUsers(); 
            printf("Varsayilan admin olusturuldu ve kaydedildi.");
        }
        return;
    }

    userCount = 0;
    while (userCount < MAX_USERS &&
           fscanf(f, "%s %s %s\n",
                  kullanicilar[userCount].kullaniciAd,
                  kullanicilar[userCount].kullaniciSifre,
                  kullanicilar[userCount].rol) == 3) {
        userCount++;
    }

    fclose(f);
    printf("Kullanici verileri yuklendi.");
}

void saveUsers() // dosyalanan kullanıcıların saklanması.
{
    FILE *f = fopen(FILENAME_USERS, "w");
    if (f == NULL) {
        printf("Kullanicilar dosyasi yazilamadi!\n");
        return;
    }

    for (int i = 0; i < userCount; i++)
    {
        fprintf(f, "%s %s %s\n",
                kullanicilar[i].kullaniciAd,
                kullanicilar[i].kullaniciSifre,
                kullanicilar[i].rol);
    }

    fclose(f);
    printf("kullanici verileri kaydedildi.\n");
}
void searchBooks() {// kayıtlı kitapları arama.
    char arama[100];
    printf("Aranacak kelime (kitap adi, yazar veya kategori): ");
    fgets(arama, sizeof(arama), stdin); //aranan kitap arama dizisine kaydedilir.
    arama[strcspn(arama, "\n")] = 0; //dizinin duzgun okunması ıcın eklendi.

    int bulundu = 0;
    printf("\n--- Arama Sonuclari ---\n");
    for (int i = 0; i < bookCount; i++) {
        if (strstr(kitaplar[i].kitapAdi, arama) != NULL || //strstr fonksiyonuyla iki dizinin ortak elemanı aranıyor.
            strstr(kitaplar[i].yazarAdi, arama) != NULL ||
            strstr(kitaplar[i].kategori, arama) != NULL) {
            printf("\nID: %s\nAdi: %s\nYazar: %s\nKategori: %s\nToplam Adet: %d\nMevcut Adet: %d\n",
                   kitaplar[i].kitapID,
                   kitaplar[i].kitapAdi,
                   kitaplar[i].yazarAdi,
                   kitaplar[i].kategori,
                   kitaplar[i].toplamAdet,
                   kitaplar[i].mevcutAdet);
            bulundu = 1;
        }
    }

    if (!bulundu) {
        printf("Aranan kriterlere uygun kitap bulunamadi.");
    }
}

void listAllBorrowedBooks() {// ödünç alınan kitapların listelenmesi.
    printf("Tum Odunc Alinan Kitaplar\n");
    if (borrowedCount == 0) {
        printf("Hic odunc alinmis kitap bulunmamaktadir.");
        return;
    }

    printf("%-10s %-15s %-10s %-20s %-15s %-15s\n",
           "Odunc ID", "Kullanici Adi", "Kitap ID", "Kitap Adi", "Odunc Tarihi", "Iade Tarihi");
    printf("--------------------------------------------------------------------------------------\n");
    for (int i=0;i<borrowedCount;i++){
        int kitapIdx=findBookIndexById(odunckitaplar[i].kitapID);
        char kitapName[100]="Bulunamadi";
        if(kitapIdx!=-1){
            strcpy(kitapName,kitaplar[kitapIdx].kitapAdi);
        } 
         printf("%-10s %-15s %-10s %-20s  %02d-%02d-%04d    %02d-%02d-%04d\n",
               odunckitaplar[i].oduncID,
               odunckitaplar[i].kullaniciAdi,
               odunckitaplar[i].kitapID,
               kitapName,
               odunckitaplar[i].oduncTarihi.gun, odunckitaplar[i].oduncTarihi.ay, odunckitaplar[i].oduncTarihi.yil,
               odunckitaplar[i].iadeTarihi.gun, odunckitaplar[i].iadeTarihi.ay, odunckitaplar[i].iadeTarihi.yil);
    
}
}
void checkOverdueBooks() { //iadesi geciken kitapların listelenmesi.
    printf("Geciken Kitaplar Kontrol Ediliyor...\n");
    bool gecikeniBul = false; //ödünç alınan kitap varlıgının true-false sistemiyle kontrolü için eklendi.
    tarih guncelTarih = getCurrentDate(); //güncel gün alınıyor.

    printf("%-10s %-15s %-10s %-20s %-15s %-15s %s\n",
           "Odunc ID", "Kullanici Adi", "Kitap ID", "Kitap Adi", "Odunc Tarihi", "Iade Tarihi", "Gecikme (Gun)");
    printf("--------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < borrowedCount; i++) {
        if (isDateGreaterThan(guncelTarih, odunckitaplar[i].iadeTarihi)) { //daha önce yazılan fonksiyon ile güncel tarih ile iade tarihi karşılaştırlıyor.
            gecikeniBul = true; // günü geciken kitap bulundugunda true dönüyor.
            int gecikenGunSayi = getDaysDifference(guncelTarih, odunckitaplar[i].iadeTarihi);// yazılan fonksiyonlarla geciken gün sayisi hesaplanıyor.
            int kitapIdx=findBookIndexById(odunckitaplar[i].kitapID);
            char kitapName[100]="Bulunamadi"; //kitapların Id sınden kıtap adı bulmak ıcın kullanılır.
            if(kitapIdx!=-1){
                strcpy(kitapName,kitaplar[kitapIdx].kitapAdi) ;           }
            printf("%-10s %-15s %-10s %-20s %02d-%02d-%04d    %02d-%02d-%04d    %d\n",
                   odunckitaplar[i].oduncID,
                   odunckitaplar[i].kullaniciAdi,
                   odunckitaplar[i].kitapID,
                   kitapName,
                   odunckitaplar[i].oduncTarihi.gun, odunckitaplar[i].oduncTarihi.ay, odunckitaplar[i].oduncTarihi.yil,
                   odunckitaplar[i].iadeTarihi.gun, odunckitaplar[i].iadeTarihi.ay, odunckitaplar[i].iadeTarihi.yil,
                   gecikenGunSayi);
        }
    }

    if (!gecikeniBul) {
        printf("Geciken kitap bulunmamaktadir.");
    }
}
void borrowBook() {// ödünç kitap alma.
  
    char *girisYapanKullanici = kullanicilar[girisYapanKullaniciIndex].kullaniciAd; //kullanıcı adına göre ödünç veriyor poınter olarak eklendi .

    int userBorrowedCount = 0;
    for (int i = 0; i < borrowedCount; i++) { // kullanicinin ödünç aldığı kitap sayısı hesaplanır
        if (strcmp(odunckitaplar[i].kullaniciAdi, girisYapanKullanici) == 0) {
            userBorrowedCount++;
        }
    }

    if (userBorrowedCount >= MAX_BORROWED_PER_USER) { 
        printf("En fazla %d kitap alabilirsiniz.\n", MAX_BORROWED_PER_USER);
        return;
    }

    char kitapIdentifier[100]; //ödünç alınan kitabın bilgilerini tutmak için oluşturuldu.
    int secim;
    printf("Kitap odunc almak icin secim yapiniz:\n");
    printf("1. Kitap ID'si ile\n");
    printf("2. Kitap adina gore\n");
    printf("Seciminiz: ");
    if (scanf("%d", &secim) != 1) { // gecerli giriş durumu kontrol edilir.
        printf("Gecersiz giris. Odunc alma iptal edildi.");
        while (getchar() != '\n');
        return;
    }
    getchar(); 

    int kitapIndex = -1;

    if (secim == 1) {
        printf("Odunc almak istediginiz kitabin ID'sini girin: ");
        fgets(kitapIdentifier, sizeof(kitapIdentifier), stdin);
        kitapIdentifier[strcspn(kitapIdentifier, "\n")] = 0;
        kitapIndex = findBookIndexById(kitapIdentifier);
    } else if (secim == 2) {
        printf("Odunc almak istediginiz kitabin adini yazin: ");
        fgets(kitapIdentifier, sizeof(kitapIdentifier), stdin);
        kitapIdentifier[strcspn(kitapIdentifier, "\n")] = 0;
        kitapIndex = findBookIndexByName(kitapIdentifier);
    } else {
        printf("Gecersiz secim.");
        return;
    }

    if (kitapIndex == -1) {
        printf("Kitap bulunamadi.");
        return;
    }

    if (kitaplar[kitapIndex].mevcutAdet <= 0) { 
        printf("Bu kitap su anda odunc alinamaz (stokta yok).");
        return;
    }

    if (borrowedCount >= MAX_BOOKS) { 
        printf("Maksimum odunc kaydina ulasildi. Lutfen yoneticiye bildirin.");
        return;
    }

    OduncKitap yeni;   //ödünç alma işlemleri burada yapılır.
    generateBorrowID(yeni.oduncID);   //ödünç kitap ıd si alınır fonksiyonla.
    strcpy(yeni.kullaniciAdi,girisYapanKullanici);   //ödünç alan kullanıcının adı alınır.
    strcpy(yeni.kitapID, kitaplar[kitapIndex].kitapID); //ödünç alınan kitabın ıd si alınır.
    yeni.oduncTarihi = getCurrentDate();  //ödünç alınma tarihi alınır.
    yeni.iadeTarihi = addDaysToDate(yeni.oduncTarihi, 14); //iade tarihi alıır.

    odunckitaplar[borrowedCount++] = yeni;
    kitaplar[kitapIndex].mevcutAdet--;  
    kitaplar[kitapIndex].oduncAlindiMi = true; 

    saveBorrowedBooks();
    saveBooks();
    printf("Kitap '%s' basariyla odunc alindi. Iade tarihi: %02d/%02d/%04d\n",
           kitaplar[kitapIndex].kitapAdi,
           yeni.iadeTarihi.gun, yeni.iadeTarihi.ay, yeni.iadeTarihi.yil);
}
void loadBorrowedBooks() {//ödünç alınan kitapalari dosyalama.
    FILE *file = fopen(FILENAME_BORROWED, "r");
    if (file == NULL) {
        printf("Odunc kitap verileri yuklenemedi veya dosya bulunamadi.");
        return;
    }

    borrowedCount = 0;
    while (borrowedCount < MAX_BOOKS && 
           fscanf(file, "%s %s %s %d-%d-%d %d-%d-%d\n",
                  odunckitaplar[borrowedCount].oduncID,
                  odunckitaplar[borrowedCount].kullaniciAdi,
                  odunckitaplar[borrowedCount].kitapID,
                  &odunckitaplar[borrowedCount].oduncTarihi.gun,
                  &odunckitaplar[borrowedCount].oduncTarihi.ay,
                  &odunckitaplar[borrowedCount].oduncTarihi.yil,
                  &odunckitaplar[borrowedCount].iadeTarihi.gun,
                  &odunckitaplar[borrowedCount].iadeTarihi.ay,
                  &odunckitaplar[borrowedCount].iadeTarihi.yil) == 9) {
        borrowedCount++;
    }
    fclose(file);
    printf("Odunc kitap verileri yuklendi.");
}
void saveBorrowedBooks() {//dosyalanan ödünç kitapların saklanması.
    FILE *file = fopen(FILENAME_BORROWED, "w");
    if (file == NULL) {
        printf("Odunc kitap verileri kaydedilemedi!");
        return;
    }

    for (int i = 0; i < borrowedCount; i++) {
        fprintf(file, "%s %s %s %02d-%02d-%04d %02d-%02d-%04d\n",
                odunckitaplar[i].oduncID,
                odunckitaplar[i].kullaniciAdi,
                odunckitaplar[i].kitapID,
                odunckitaplar[i].oduncTarihi.gun,
                odunckitaplar[i].oduncTarihi.ay,
                odunckitaplar[i].oduncTarihi.yil,
                odunckitaplar[i].iadeTarihi.gun,
                odunckitaplar[i].iadeTarihi.ay,
                odunckitaplar[i].iadeTarihi.yil);
    }
    fclose(file);
    printf("Odunc kitap verileri kaydedildi.");
}

void returnBook() {//kitap iadesi.
    char *girisYapanKullanici = kullanicilar[girisYapanKullaniciIndex].kullaniciAd; // ödünç kullanici adına göre verildiği kullanıcı adıyla iade edilecek.
    bool oduncBul = false; //ödünç alımmış kitap varsa true yoksa false dönecek.

    printf("\n--- Odunc Aldiginiz Kitaplar ---\n"); //iade ederken kolaylık olaması kullanıcının hangi kitaplari ödünç aldığı listeleniyor.
    for (int i = 0; i < borrowedCount; i++) {
        if (strcmp(odunckitaplar[i].kullaniciAdi,girisYapanKullanici) == 0) {
            int kitapIdx = findBookIndexById(odunckitaplar[i].kitapID);
            if (kitapIdx != -1) {
                printf("Odunc ID: %s | Kitap ID: %s | Ad: %s | Odunc Tarihi: %02d/%02d/%04d | Iade Tarihi: %02d/%02d/%04d\n",
                       odunckitaplar[i].oduncID,
                       kitaplar[kitapIdx].kitapID,
                       kitaplar[kitapIdx].kitapAdi,
                       odunckitaplar[i].oduncTarihi.gun, odunckitaplar[i].oduncTarihi.ay, odunckitaplar[i].oduncTarihi.yil,
                       odunckitaplar[i].iadeTarihi.gun, odunckitaplar[i].iadeTarihi.ay, odunckitaplar[i].iadeTarihi.yil);
                oduncBul = true;
            }
        }
    }

    if (!oduncBul) {
     printf("Iade edilecek odunc alinmis kitabiniz bulunmamaktadir.");
        return;
    }

    char borrowIDToReturn[10]; //iade edilecek kitabın bılgılerı için oluşturuldu.
    printf("Iade etmek istediginiz kitabin Odunc ID'sini girin: ");
    fgets(borrowIDToReturn, sizeof(borrowIDToReturn), stdin);
    borrowIDToReturn[strcspn(borrowIDToReturn, "\n")] = 0;

    int oduncKayitIndex = -1;
    for (int i = 0; i < borrowedCount; i++) { //kitap iadesi için girilen bilgilerle odunc kitap bılgılerı karsılastırılıyor.
        if (strcmp(odunckitaplar[i].oduncID, borrowIDToReturn) == 0 &&
            strcmp(odunckitaplar[i].kullaniciAdi,girisYapanKullanici) == 0) {
            oduncKayitIndex = i;
            break;
        }
    }

    if (oduncKayitIndex == -1) {
        printf("Gecersiz Odunc ID veya bu odunc kaydi size ait degil.");
        return;
    }

    int kitapIndex = findBookIndexById(odunckitaplar[oduncKayitIndex].kitapID); //iadesi yapılıyor
    if (kitapIndex != -1) {
        kitaplar[kitapIndex].mevcutAdet++;
        
        if (kitaplar[kitapIndex].mevcutAdet == kitaplar[kitapIndex].toplamAdet) {
            kitaplar[kitapIndex].oduncAlindiMi = false;
        }
    }

    for (int i =oduncKayitIndex; i < borrowedCount - 1; i++) {
        odunckitaplar[i] = odunckitaplar[i + 1];
    }
    borrowedCount--;

    saveBorrowedBooks();
    saveBooks();
    printf("Kitap basariyla iade edildi.");
}
void viewBorrowedBooks() {// ödünç alınan kitapların görüntülenmesi.
  
    char *girisYapanKullanici = kullanicilar[girisYapanKullaniciIndex].kullaniciAd;
    bool oduncBul = false;  //ödünç alımmış kitap varsa true yoksa false dönecek.

    printf("Odunc Aldiginiz Kitaplar\n");
    printf("%-10s %-10s %-20s %-15s %-15s\n",
           "Odunc ID", "Kitap ID", "Kitap Adi", "Odunc Tarihi", "Iade Tarihi");
    printf("-------------------------------------------------------------------------------\n");

    for (int i = 0; i < borrowedCount; i++) {
        if (strcmp(odunckitaplar[i].kullaniciAdi, girisYapanKullanici) == 0) { // bu satırda giriş yapan kullanıcı ıle odunc kitap alan kullanjıcılar karsılastırılıyor giris yapan kullanıcının odunc aldıgı kıtap olup olmadığını kontrol için.
         int kitapIdx=findBookIndexById(odunckitaplar[i].kitapID);
         char kitapName[100]="Bulunamadi";
         if(kitapIdx!=-1){
            strcpy(kitapName,kitaplar[kitapIdx].kitapAdi);
         }  
           
            printf("%-10s %-10s %-20s %02d-%02d-%04d    %02d-%02d-%04d\n",
                   odunckitaplar[i].oduncID,
                   odunckitaplar[i].kitapID,
                   kitapName,
                   odunckitaplar[i].oduncTarihi.gun, odunckitaplar[i].oduncTarihi.ay, odunckitaplar[i].oduncTarihi.yil,
                   odunckitaplar[i].iadeTarihi.gun, odunckitaplar[i].iadeTarihi.ay, odunckitaplar[i].iadeTarihi.yil);
            oduncBul = true;
        }
    }

    if (!oduncBul) {
        printf("Su anda odunc aldiginiz kitap yok.");
    }
}