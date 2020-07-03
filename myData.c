#include "sys/wait.h"
#include "termio.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"

#define BUFFER_SIZE 500         //Eğer bir limit koymaz isek verilen paragraf çok uzun olunca terminal bunu kendisi bölebilir, bu yüzden sınırlandırıyoruz
#define MAXIMUM_LINE_NUMBER 200 //Maksimum 200 satırlık text file okunabiliyor
#define LINES_PER_TURN 24       //Verilen text dosyası içinde en az 24 satır olmalı
#define READ_END 0
#define WRITE_END 1

//Bu fonksiyon ile kullanıcıdan 'q' ve 'space' gibi girdileri alıyoruz
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

    return ch;
}

//Bu fonksiyon bize verilen text dosyasının byte cinsinden boyutunu hesaplayıp döndürüyor
int fileSize(FILE *fp)
{
    fseek(fp, 0L, SEEK_END);
    int fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    return fileSize;
}

//Bu fonksiyon bize file pointerı döndürüyor
FILE *getFile(char *filePath)
{
    FILE *fp;
    fp = fopen(filePath, "r");

    if (fp != NULL)
        return fp;

    else
    {
        perror("Error");
        exit(1);
    }
}

//Bu fonksiyon verilen text dosyasını aynı anda hem okuyup hem de ekranda görüntülememizi sağlıyor.
int readAllFile(FILE *fp)
{

    int byteOfFile = fileSize(fp);
    char *records = malloc(byteOfFile + 1);

    if (!fp)
        return -1;

    while (fgets(records, byteOfFile, fp) != NULL) //Dosya sonu gelene kadar text dosyasını oku
        printf("%s", records);

    fclose(fp);

    return 0;
}

int main(int argc, char *argv[])
{

    FILE *fp = getFile(argv[1]);

    if (argc == 2)
    { //Eğer = operatörü kullanılmadıysa text dosyasını okurken ekrana basan readAllFile fonksiyonumuzu çağırır
        readAllFile(fp);
    }

    else if (!strcmp(argv[2], "=") && !strcmp(argv[3], "myMore"))
    { // = Operatörü ile pipe myMore verildiği zaman, dosyayı 24 satır halinde okuyup myMore a gönder

        char buffer[BUFFER_SIZE];
        char inputChar;                                   //Kullanıcının girdiği 'q' ve 'space' tuşlarını tuttuğumuz değişken
        int isFinished = 0;                               //Dosya sonunun geldiğini belirleyeceğimiz integer değişkenimiz
        char write_msg[MAXIMUM_LINE_NUMBER][BUFFER_SIZE]; //Pipe kullanarak belge içeriğini gönder

        do
        {

            pid_t pid;
            int fd[2];
            pipe(fd);     // Pipe oluşturduk
            pid = fork(); // İkinci process oluşturuldu

            if (pid < 0)
            { // pid sıfırdan küçük ise fork işlemi başarısız olmuştur
                fprintf(stderr, "Fork Failed");
                return 1;
            }

            if (pid == 0)
            { // Child process

                close(fd[WRITE_END]);

                char read_end_pipe[10]; //Pipe ile son gönderilen noktayı sakladığımız değişken
                sprintf(read_end_pipe, "%d", fd[READ_END]);

                char *arguments[3] = {argv[3], read_end_pipe, NULL};
                if (execv(argv[3], arguments) == -1)
                {
                    perror("Client başlatılırken hata !");
                    return -1;
                }
            }

            if (pid > 0)
            {                      // Parent process
                int lineCount = 0; // Kaç satır okuduğumuzu tuttuğumuz integer değişkenimiz
                for (int i = 0; i < LINES_PER_TURN; ++i)
                { // 24 satır okuyacağız
                    if (fgets(buffer, sizeof(buffer), fp) > 0)
                    { // Satır satır okuma
                        strcpy(write_msg[i], buffer);
                        lineCount++;
                    }
                    else
                    { // Dosya tamamlandıysa isFinished değerini 1 yapıp dosya okuma işlemini bitiriyoruz
                        isFinished = 1;
                        break;
                    }
                }

                close(fd[READ_END]);
                write(fd[WRITE_END], &write_msg, lineCount * BUFFER_SIZE);
                close(fd[WRITE_END]);

                wait(NULL);
            }

            if (isFinished == 0)
                printf("----------Devam Etmek İçin Space, Çıkmak İçin q ya Basın----------\n");
            else
            {
                printf("----------Dosya Okuma Tamamlandı----------\n");
                return 0;
            }

            inputChar = getch(); // Kullanıcının girdiği q ya da space tuşunu alıyoruz ve q girilirse do while dan çıkıyoruz

        } while (isFinished != 1 && inputChar != 'q');
    }

    else
    {
        printf("Hatalı giriş yaptınız. Programı derlemek ve çalıştırmak için gerekli yönergelere Rapordan ulaşabilirsiniz.\n");
        return 1;
    }

    return 0;
}
