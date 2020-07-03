# Pipe
Problem Tanımı

myData.c programımız, argv[1] parametresinde verilen txt uzantılı dokümanı satır satır okuyarak ekrana basacaktır. myMore.c programımız ise myData.c programının argv[1]  parametresinde verilen dokümanı aralarında kurulan pipe üzerinden okuyacak ve 24 er satır halinde bölerek ekranda görüntüleyecektir. Her 24 satır sonunda kullanıcıdan bir karakter istenip devam edilip edilmeyeceği belirlenecektir. 

Ödevin Amacı

Geliştirdiğimiz programlarımızın amacı Linux İşletim Sistemi üzerinde pipe çalışma mantığını somut bir şekilde görebilmek ve bu işlemleri modelleyebilmektir. Bu noktada geliştirilen myData.c programımız tek başına bir text dosyasından satır satır veri okumakta ve “=” operatörü kullanıldığında pipe ile (./myData inputFile.txt = myMore) okuduğu verileri myMore.c programımıza gönderip, ekranda 24 er satır halinde görüntülenmesini sağlamaktadır. Geliştirilen bu ödev ile Linux İşletim Sistemi üzerinde pipe işlemleri net bir şekilde görülebilmekte ve anlaşılabilmektedir.

Ödevin Kapsamı

Burada 2 program arasında veri gönderebilmek için pipe kullanacağız. Linux çekirdeği üzerinden işlemler arasında "pipe()" fonksiyonu (sistem çağrısı) ile bir iletişim kanalı kurulabilmektedir. İletişim 2 şekilde gerçekleşir:

1.	Ana işlem -> Çekirdek -> Yan işlem:  Ana işlemden gönderilen mesajı çekirdek yan işleme iletiyor.

2.	Ana işlem <- Çekirdek <- Yan işlem: Yan işlemden gönderilen mesajı çekirdek ana işleme iletiyor.






Şöyle bir şemamız bulunmakta:

 
Parent  in     <=            KERNEL       =>  in
Process out    =>                         <=  out

Yani çekirdek bir tarafın çıktısı ile diğer tarafın girdisini birbirine bağlıyor. Bu kanal üzerinden mesaj göndermek için "write()", okumak içinse "read()" fonksiyonları (sistem çağrıları) kullanıyoruz.
