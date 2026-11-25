.class public Lio/github/vitarecomps/frontend/FrontendStepActivity;
.super Lio/github/vitarecomps/frontend/GameActivity;
.source "FrontendStepActivity.java"


# static fields
.field public static final EXTRA_EXIT_CODE:Ljava/lang/String; = "exit_code"


# instance fields
.field private importError:Ljava/lang/String;

.field private final picked:Ljava/util/List;
    .annotation system Ldalvik/annotation/Signature;
        value = {
            "Ljava/util/List<",
            "Landroid/os/ParcelFileDescriptor;",
            ">;"
        }
    .end annotation
.end field

.field private prepared:[Ljava/lang/String;


# direct methods
.method public constructor <init>()V
    .locals 1

    .line 3
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/GameActivity;-><init>()V

    .line 4
    new-instance v0, Ljava/util/ArrayList;

    invoke-direct {v0}, Ljava/util/ArrayList;-><init>()V

    iput-object v0, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->picked:Ljava/util/List;

    return-void
.end method

.method private check(Ljava/io/File;)V
    .locals 5
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .line 6
    new-instance v0, Ljava/io/RandomAccessFile;

    const-string v1, "r"

    invoke-direct {v0, p1, v1}, Ljava/io/RandomAccessFile;-><init>(Ljava/io/File;Ljava/lang/String;)V

    :try_start_0
    invoke-virtual {v0}, Ljava/io/RandomAccessFile;->length()J

    move-result-wide v1

    const-wide/16 v3, 0x0

    cmp-long p1, v1, v3

    if-eqz p1, :cond_1

    invoke-virtual {v0, v3, v4}, Ljava/io/RandomAccessFile;->seek(J)V

    invoke-virtual {v0}, Ljava/io/RandomAccessFile;->read()I

    move-result p1

    if-ltz p1, :cond_0

    invoke-virtual {v0, v3, v4}, Ljava/io/RandomAccessFile;->seek(J)V
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    invoke-virtual {v0}, Ljava/io/RandomAccessFile;->close()V

    return-void

    :cond_0
    :try_start_1
    new-instance p1, Ljava/io/IOException;

    const-string v1, "Nao foi possivel ler o arquivo"

    invoke-direct {p1, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    :goto_0
    throw p1

    :cond_1
    new-instance p1, Ljava/io/IOException;

    const-string v1, "O arquivo esta vazio"

    invoke-direct {p1, v1}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    goto :goto_0

    :catchall_0
    move-exception p1

    :try_start_2
    invoke-virtual {v0}, Ljava/io/RandomAccessFile;->close()V
    :try_end_2
    .catchall {:try_start_2 .. :try_end_2} :catchall_1

    goto :goto_1

    :catchall_1
    move-exception v0

    invoke-virtual {p1, v0}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_1
    throw p1
.end method

.method static synthetic lambda$onDestroy$0()V
    .locals 1

    .line 10
    invoke-static {}, Landroid/os/Process;->myPid()I

    move-result v0

    invoke-static {v0}, Landroid/os/Process;->killProcess(I)V

    return-void
.end method

.method private local(Landroid/net/Uri;Ljava/lang/String;IJ)Ljava/io/File;
    .locals 7
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .line 7
    new-instance v0, Ljava/io/File;

    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->getCacheDir()Ljava/io/File;

    move-result-object v1

    const-string v2, "import-staging"

    invoke-direct {v0, v1, v2}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-virtual {v0}, Ljava/io/File;->isDirectory()Z

    move-result v1

    if-nez v1, :cond_1

    invoke-virtual {v0}, Ljava/io/File;->mkdirs()Z

    move-result v1

    if-eqz v1, :cond_0

    goto :goto_0

    :cond_0
    new-instance p1, Ljava/io/IOException;

    const-string p2, "Nao foi possivel criar a pasta temporaria"

    invoke-direct {p1, p2}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw p1

    :cond_1
    :goto_0
    const-wide/16 v1, 0x0

    cmp-long v1, p4, v1

    const-string v2, " bytes"

    if-lez v1, :cond_3

    invoke-virtual {v0}, Ljava/io/File;->getUsableSpace()J

    move-result-wide v3

    const-wide/32 v5, 0x4000000

    add-long/2addr v5, p4

    cmp-long v1, v3, v5

    if-ltz v1, :cond_2

    goto :goto_1

    :cond_2
    new-instance p1, Ljava/io/IOException;

    new-instance p2, Ljava/lang/StringBuilder;

    invoke-direct {p2}, Ljava/lang/StringBuilder;-><init>()V

    const-string p3, "Espaco insuficiente para copiar o arquivo: "

    invoke-virtual {p2, p3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p2

    invoke-virtual {p2, p4, p5}, Ljava/lang/StringBuilder;->append(J)Ljava/lang/StringBuilder;

    move-result-object p2

    invoke-virtual {p2, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p2

    invoke-virtual {p2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p2

    invoke-direct {p1, p2}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw p1

    :cond_3
    :goto_1
    new-instance p4, Ljava/io/File;

    new-instance p5, Ljava/lang/StringBuilder;

    invoke-direct {p5}, Ljava/lang/StringBuilder;-><init>()V

    const-string v1, "picked-"

    invoke-virtual {p5, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p5

    invoke-virtual {p5, p3}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object p5

    invoke-virtual {p5, p2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p5

    const-string v3, ".part"

    invoke-virtual {p5, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p5

    invoke-virtual {p5}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p5

    invoke-direct {p4, v0, p5}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    new-instance p5, Ljava/io/File;

    new-instance v3, Ljava/lang/StringBuilder;

    invoke-direct {v3}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual {v3, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v1

    invoke-virtual {v1, p3}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object p3

    invoke-virtual {p3, p2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p2

    invoke-virtual {p2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p2

    invoke-direct {p5, v0, p2}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    :try_start_0
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->getContentResolver()Landroid/content/ContentResolver;

    move-result-object p2

    invoke-virtual {p2, p1}, Landroid/content/ContentResolver;->openInputStream(Landroid/net/Uri;)Ljava/io/InputStream;

    move-result-object p1
    :try_end_0
    .catch Ljava/io/IOException; {:try_start_0 .. :try_end_0} :catch_0

    :try_start_1
    new-instance p2, Ljava/io/FileOutputStream;

    invoke-direct {p2, p4}, Ljava/io/FileOutputStream;-><init>(Ljava/io/File;)V
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_2

    if-eqz p1, :cond_6

    const/high16 p3, 0x100000

    :try_start_2
    new-array p3, p3, [B

    :goto_2
    invoke-virtual {p1, p3}, Ljava/io/InputStream;->read([B)I

    move-result v0

    const/4 v1, -0x1

    const/4 v3, 0x0

    if-eq v0, v1, :cond_4

    invoke-virtual {p2, p3, v3, v0}, Ljava/io/OutputStream;->write([BII)V
    :try_end_2
    .catchall {:try_start_2 .. :try_end_2} :catchall_0

    goto :goto_2

    :cond_4
    :try_start_3
    invoke-virtual {p2}, Ljava/io/OutputStream;->close()V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_2

    if-eqz p1, :cond_5

    :try_start_4
    invoke-virtual {p1}, Ljava/io/InputStream;->close()V
    :try_end_4
    .catch Ljava/io/IOException; {:try_start_4 .. :try_end_4} :catch_0

    :cond_5
    invoke-virtual {p4}, Ljava/io/File;->toPath()Ljava/nio/file/Path;

    move-result-object p1

    invoke-virtual {p5}, Ljava/io/File;->toPath()Ljava/nio/file/Path;

    move-result-object p2

    const/4 p3, 0x1

    new-array p3, p3, [Ljava/nio/file/CopyOption;

    sget-object p4, Ljava/nio/file/StandardCopyOption;->REPLACE_EXISTING:Ljava/nio/file/StandardCopyOption;

    aput-object p4, p3, v3

    invoke-static {p1, p2, p3}, Ljava/nio/file/Files;->move(Ljava/nio/file/Path;Ljava/nio/file/Path;[Ljava/nio/file/CopyOption;)Ljava/nio/file/Path;

    invoke-direct {p0, p5}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->check(Ljava/io/File;)V

    new-instance p1, Ljava/lang/StringBuilder;

    invoke-direct {p1}, Ljava/lang/StringBuilder;-><init>()V

    const-string p2, "Copia local preparada: "

    invoke-virtual {p1, p2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p5}, Ljava/io/File;->getName()Ljava/lang/String;

    move-result-object p2

    invoke-virtual {p1, p2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    const-string p2, ", "

    invoke-virtual {p1, p2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p5}, Ljava/io/File;->length()J

    move-result-wide p2

    invoke-virtual {p1, p2, p3}, Ljava/lang/StringBuilder;->append(J)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p1

    invoke-static {p0, p1}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->note(Landroid/content/Context;Ljava/lang/String;)V

    return-object p5

    :catchall_0
    move-exception p3

    goto :goto_3

    :cond_6
    :try_start_5
    new-instance p3, Ljava/io/IOException;

    const-string p5, "Android nao forneceu acesso de leitura"

    invoke-direct {p3, p5}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw p3
    :try_end_5
    .catchall {:try_start_5 .. :try_end_5} :catchall_0

    :goto_3
    :try_start_6
    invoke-virtual {p2}, Ljava/io/OutputStream;->close()V
    :try_end_6
    .catchall {:try_start_6 .. :try_end_6} :catchall_1

    goto :goto_4

    :catchall_1
    move-exception p2

    :try_start_7
    invoke-virtual {p3, p2}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_4
    throw p3
    :try_end_7
    .catchall {:try_start_7 .. :try_end_7} :catchall_2

    :catchall_2
    move-exception p2

    if-eqz p1, :cond_7

    :try_start_8
    invoke-virtual {p1}, Ljava/io/InputStream;->close()V
    :try_end_8
    .catchall {:try_start_8 .. :try_end_8} :catchall_3

    goto :goto_5

    :catchall_3
    move-exception p1

    :try_start_9
    invoke-virtual {p2, p1}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_7
    :goto_5
    throw p2
    :try_end_9
    .catch Ljava/io/IOException; {:try_start_9 .. :try_end_9} :catch_0

    :catch_0
    move-exception p1

    invoke-virtual {p4}, Ljava/io/File;->delete()Z

    throw p1
.end method

.method private name(Landroid/net/Uri;)Ljava/lang/String;
    .locals 6

    .line 5
    :try_start_0
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->getContentResolver()Landroid/content/ContentResolver;

    move-result-object v0

    const-string v1, "_display_name"

    filled-new-array {v1}, [Ljava/lang/String;

    move-result-object v2

    const/4 v4, 0x0

    const/4 v5, 0x0

    const/4 v3, 0x0

    move-object v1, p1

    invoke-virtual/range {v0 .. v5}, Landroid/content/ContentResolver;->query(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;

    move-result-object v0
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    if-eqz v0, :cond_2

    :try_start_1
    invoke-interface {v0}, Landroid/database/Cursor;->moveToFirst()Z

    move-result v1

    if-eqz v1, :cond_2

    const/4 v1, 0x0

    invoke-interface {v0, v1}, Landroid/database/Cursor;->getString(I)Ljava/lang/String;

    move-result-object v2

    if-eqz v2, :cond_2

    invoke-interface {v0, v1}, Landroid/database/Cursor;->getString(I)Ljava/lang/String;

    move-result-object v1
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    if-eqz v0, :cond_0

    :try_start_2
    invoke-interface {v0}, Landroid/database/Cursor;->close()V
    :try_end_2
    .catch Ljava/lang/Exception; {:try_start_2 .. :try_end_2} :catch_0

    :cond_0
    return-object v1

    :catchall_0
    move-exception v1

    if-eqz v0, :cond_1

    :try_start_3
    invoke-interface {v0}, Landroid/database/Cursor;->close()V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_1

    goto :goto_0

    :catchall_1
    move-exception v0

    :try_start_4
    invoke-virtual {v1, v0}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_1
    :goto_0
    throw v1

    :cond_2
    if-eqz v0, :cond_3

    invoke-interface {v0}, Landroid/database/Cursor;->close()V
    :try_end_4
    .catch Ljava/lang/Exception; {:try_start_4 .. :try_end_4} :catch_0

    goto :goto_1

    :catch_0
    move-exception v0

    new-instance v1, Ljava/lang/StringBuilder;

    invoke-direct {v1}, Ljava/lang/StringBuilder;-><init>()V

    const-string v2, "Nome do documento indisponivel: "

    invoke-virtual {v1, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v1

    invoke-virtual {v1, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/Object;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    invoke-static {p0, v0}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->note(Landroid/content/Context;Ljava/lang/String;)V

    :cond_3
    :goto_1
    invoke-virtual {p1}, Landroid/net/Uri;->getLastPathSegment()Ljava/lang/String;

    move-result-object v0

    if-eqz v0, :cond_4

    invoke-virtual {p1}, Landroid/net/Uri;->getLastPathSegment()Ljava/lang/String;

    move-result-object p1

    goto :goto_2

    :cond_4
    const-string p1, "arquivo"

    :goto_2
    return-object p1
.end method


# virtual methods
.method protected getArguments()[Ljava/lang/String;
    .locals 14

    .line 8
    const-string v0, ": "

    iget-object v1, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->prepared:[Ljava/lang/String;

    if-eqz v1, :cond_0

    iget-object v0, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->prepared:[Ljava/lang/String;

    return-object v0

    :cond_0
    invoke-super {p0}, Lio/github/vitarecomps/frontend/GameActivity;->getArguments()[Ljava/lang/String;

    move-result-object v1

    invoke-virtual {v1}, [Ljava/lang/String;->clone()Ljava/lang/Object;

    move-result-object v1

    check-cast v1, [Ljava/lang/String;

    const/4 v2, 0x0

    :goto_0
    array-length v3, v1

    if-ge v2, v3, :cond_c

    aget-object v3, v1, v2

    const-string v4, "content://"

    invoke-virtual {v3, v4}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z

    move-result v3

    if-nez v3, :cond_2

    :cond_1
    :goto_1
    goto/16 :goto_7

    :cond_2
    aget-object v3, v1, v2

    invoke-static {v3}, Landroid/net/Uri;->parse(Ljava/lang/String;)Landroid/net/Uri;

    move-result-object v4

    invoke-direct {p0, v4}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->name(Landroid/net/Uri;)Ljava/lang/String;

    move-result-object v9

    const/16 v3, 0x2e

    invoke-virtual {v9, v3}, Ljava/lang/String;->lastIndexOf(I)I

    move-result v3

    if-ltz v3, :cond_3

    invoke-virtual {v9, v3}, Ljava/lang/String;->substring(I)Ljava/lang/String;

    move-result-object v3

    sget-object v5, Ljava/util/Locale;->ROOT:Ljava/util/Locale;

    invoke-virtual {v3, v5}, Ljava/lang/String;->toLowerCase(Ljava/util/Locale;)Ljava/lang/String;

    move-result-object v3

    goto :goto_2

    :cond_3
    const-string v3, ""

    :goto_2
    move-object v5, v3

    const/4 v3, 0x0

    :try_start_0
    new-instance v6, Ljava/lang/StringBuilder;

    invoke-direct {v6}, Ljava/lang/StringBuilder;-><init>()V

    const-string v7, "Abrindo documento "

    invoke-virtual {v6, v7}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v6

    invoke-virtual {v6, v9}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v6

    const-string v7, " como argumento "

    invoke-virtual {v6, v7}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v6

    invoke-virtual {v6, v2}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v6

    invoke-virtual {v6}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v6

    invoke-static {p0, v6}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->note(Landroid/content/Context;Ljava/lang/String;)V

    invoke-virtual {v5}, Ljava/lang/String;->length()I

    move-result v6

    const/16 v7, 0xc

    if-gt v6, v7, :cond_a

    const-string v6, "\\.[a-z0-9]+"

    invoke-virtual {v5, v6}, Ljava/lang/String;->matches(Ljava/lang/String;)Z

    move-result v6

    if-eqz v6, :cond_a

    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->getContentResolver()Landroid/content/ContentResolver;

    move-result-object v6

    const-string v7, "r"

    invoke-virtual {v6, v4, v7}, Landroid/content/ContentResolver;->openFileDescriptor(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;

    move-result-object v10
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_4
    .catchall {:try_start_0 .. :try_end_0} :catchall_1

    if-eqz v10, :cond_9

    :try_start_1
    invoke-virtual {v10}, Landroid/os/ParcelFileDescriptor;->getStatSize()J

    move-result-wide v7

    const-wide/16 v11, 0x0

    cmp-long v6, v7, v11

    if-eqz v6, :cond_8

    const-string v6, ".bin"

    invoke-virtual {v5, v6}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result v6

    if-nez v6, :cond_7

    const-string v6, ".rif"

    invoke-virtual {v5, v6}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result v6

    if-eqz v6, :cond_4

    goto/16 :goto_6

    :cond_4
    new-instance v6, Ljava/io/File;

    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->getCacheDir()Ljava/io/File;

    move-result-object v11

    new-instance v12, Ljava/lang/StringBuilder;

    invoke-direct {v12}, Ljava/lang/StringBuilder;-><init>()V

    const-string v13, "picked-"

    invoke-virtual {v12, v13}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v12

    invoke-virtual {v12, v2}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v12

    invoke-virtual {v12, v5}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v12

    invoke-virtual {v12}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v12

    invoke-direct {v6, v11, v12}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V
    :try_end_1
    .catch Ljava/lang/Exception; {:try_start_1 .. :try_end_1} :catch_3
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    :try_start_2
    invoke-virtual {v6}, Ljava/io/File;->exists()Z

    move-result v11

    if-eqz v11, :cond_6

    invoke-virtual {v6}, Ljava/io/File;->delete()Z

    move-result v11

    if-eqz v11, :cond_5

    goto :goto_3

    :cond_5
    new-instance v3, Ljava/io/IOException;

    const-string v6, "Nao foi possivel substituir o caminho temporario"

    invoke-direct {v3, v6}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v3

    :cond_6
    :goto_3
    new-instance v11, Ljava/lang/StringBuilder;

    invoke-direct {v11}, Ljava/lang/StringBuilder;-><init>()V

    const-string v12, "/proc/self/fd/"

    invoke-virtual {v11, v12}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v11

    invoke-virtual {v10}, Landroid/os/ParcelFileDescriptor;->getFd()I

    move-result v12

    invoke-virtual {v11, v12}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v11

    invoke-virtual {v11}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v11

    invoke-virtual {v6}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v12

    invoke-static {v11, v12}, Landroid/system/Os;->symlink(Ljava/lang/String;Ljava/lang/String;)V

    invoke-direct {p0, v6}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->check(Ljava/io/File;)V

    invoke-virtual {v6}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v6

    aput-object v6, v1, v2

    iget-object v6, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->picked:Ljava/util/List;

    invoke-interface {v6, v10}, Ljava/util/List;->add(Ljava/lang/Object;)Z
    :try_end_2
    .catch Ljava/lang/Exception; {:try_start_2 .. :try_end_2} :catch_1
    .catchall {:try_start_2 .. :try_end_2} :catchall_0

    :try_start_3
    new-instance v6, Ljava/lang/StringBuilder;

    invoke-direct {v6}, Ljava/lang/StringBuilder;-><init>()V

    const-string v10, "Acesso direto validado: "

    invoke-virtual {v6, v10}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v6

    invoke-virtual {v6, v9}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v6

    const-string v10, "; tamanho "

    invoke-virtual {v6, v10}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v6

    invoke-virtual {v6, v7, v8}, Ljava/lang/StringBuilder;->append(J)Ljava/lang/StringBuilder;

    move-result-object v6

    invoke-virtual {v6}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v6

    invoke-static {p0, v6}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->note(Landroid/content/Context;Ljava/lang/String;)V
    :try_end_3
    .catch Ljava/lang/Exception; {:try_start_3 .. :try_end_3} :catch_0
    .catchall {:try_start_3 .. :try_end_3} :catchall_1

    goto :goto_5

    :catch_0
    move-exception v6

    move-object v10, v3

    move-object v3, v6

    goto :goto_4

    :catch_1
    move-exception v3

    :goto_4
    :try_start_4
    new-instance v6, Ljava/lang/StringBuilder;

    invoke-direct {v6}, Ljava/lang/StringBuilder;-><init>()V

    const-string v11, "Ponte por descritor falhou para "

    invoke-virtual {v6, v11}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v6

    invoke-virtual {v6, v9}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v6

    invoke-virtual {v6, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v6

    invoke-virtual {v6, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/Object;)Ljava/lang/StringBuilder;

    move-result-object v3

    const-string v6, "; tentando copia local"

    invoke-virtual {v3, v6}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v3

    invoke-static {p0, v3}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->note(Landroid/content/Context;Ljava/lang/String;)V

    move-object v3, p0

    move v6, v2

    invoke-direct/range {v3 .. v8}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->local(Landroid/net/Uri;Ljava/lang/String;IJ)Ljava/io/File;

    move-result-object v3

    invoke-virtual {v3}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v3

    aput-object v3, v1, v2
    :try_end_4
    .catch Ljava/lang/Exception; {:try_start_4 .. :try_end_4} :catch_3
    .catchall {:try_start_4 .. :try_end_4} :catchall_0

    move-object v3, v10

    :goto_5
    if-eqz v3, :cond_1

    :try_start_5
    invoke-virtual {v3}, Landroid/os/ParcelFileDescriptor;->close()V
    :try_end_5
    .catch Ljava/io/IOException; {:try_start_5 .. :try_end_5} :catch_2

    goto/16 :goto_1

    :catch_2
    move-exception v3

    goto/16 :goto_1

    :cond_7
    :goto_6
    move-object v3, p0

    move v6, v2

    :try_start_6
    invoke-direct/range {v3 .. v8}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->local(Landroid/net/Uri;Ljava/lang/String;IJ)Ljava/io/File;

    move-result-object v3

    invoke-virtual {v3}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v3

    aput-object v3, v1, v2

    invoke-virtual {v10}, Landroid/os/ParcelFileDescriptor;->close()V

    goto/16 :goto_1

    :goto_7
    add-int/lit8 v2, v2, 0x1

    goto/16 :goto_0

    :cond_8
    new-instance v2, Ljava/io/IOException;

    new-instance v3, Ljava/lang/StringBuilder;

    invoke-direct {v3}, Ljava/lang/StringBuilder;-><init>()V

    const-string v4, "Arquivo vazio: "

    invoke-virtual {v3, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3, v9}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v3

    invoke-direct {v2, v3}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    :goto_8
    throw v2

    :catchall_0
    move-exception v0

    move-object v3, v10

    goto :goto_a

    :catch_3
    move-exception v2

    move-object v3, v10

    goto :goto_9

    :cond_9
    new-instance v2, Ljava/io/IOException;

    const-string v3, "Android retornou descritor de arquivo nulo"

    invoke-direct {v2, v3}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V
    :try_end_6
    .catch Ljava/lang/Exception; {:try_start_6 .. :try_end_6} :catch_3
    .catchall {:try_start_6 .. :try_end_6} :catchall_0

    goto :goto_8

    :cond_a
    :try_start_7
    new-instance v2, Ljava/io/IOException;

    new-instance v4, Ljava/lang/StringBuilder;

    invoke-direct {v4}, Ljava/lang/StringBuilder;-><init>()V

    const-string v5, "Extensao nao reconhecida: "

    invoke-virtual {v4, v5}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v4, v9}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v4}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v4

    invoke-direct {v2, v4}, Ljava/io/IOException;-><init>(Ljava/lang/String;)V

    throw v2
    :try_end_7
    .catch Ljava/lang/Exception; {:try_start_7 .. :try_end_7} :catch_4
    .catchall {:try_start_7 .. :try_end_7} :catchall_1

    :catchall_1
    move-exception v0

    goto :goto_a

    :catch_4
    move-exception v2

    :goto_9
    :try_start_8
    new-instance v4, Ljava/lang/StringBuilder;

    invoke-direct {v4}, Ljava/lang/StringBuilder;-><init>()V

    const-string v5, "Nao foi possivel preparar "

    invoke-virtual {v4, v5}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v4, v9}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v4, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v2}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v5

    invoke-virtual {v5}, Ljava/lang/Class;->getSimpleName()Ljava/lang/String;

    move-result-object v5

    invoke-virtual {v4, v5}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v4, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v2}, Ljava/lang/Exception;->getMessage()Ljava/lang/String;

    move-result-object v2

    invoke-virtual {v0, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    iput-object v0, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->importError:Ljava/lang/String;

    iget-object v0, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->importError:Ljava/lang/String;

    invoke-static {p0, v0}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->note(Landroid/content/Context;Ljava/lang/String;)V
    :try_end_8
    .catchall {:try_start_8 .. :try_end_8} :catchall_1

    if-eqz v3, :cond_c

    :try_start_9
    invoke-virtual {v3}, Landroid/os/ParcelFileDescriptor;->close()V
    :try_end_9
    .catch Ljava/io/IOException; {:try_start_9 .. :try_end_9} :catch_5

    goto :goto_c

    :catch_5
    move-exception v0

    goto :goto_c

    :goto_a
    if-eqz v3, :cond_b

    :try_start_a
    invoke-virtual {v3}, Landroid/os/ParcelFileDescriptor;->close()V
    :try_end_a
    .catch Ljava/io/IOException; {:try_start_a .. :try_end_a} :catch_6

    goto :goto_b

    :catch_6
    move-exception v1

    :cond_b
    :goto_b
    throw v0

    :cond_c
    :goto_c
    iput-object v1, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->prepared:[Ljava/lang/String;

    return-object v1
.end method

.method protected main()V
    .locals 5

    .line 9
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->getArguments()[Ljava/lang/String;

    move-result-object v0

    iget-object v1, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->importError:Ljava/lang/String;

    const/4 v2, 0x1

    if-nez v1, :cond_0

    :try_start_0
    const-string v1, "Iniciando etapa nativa; arquivos acessiveis."

    invoke-static {p0, v1}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->note(Landroid/content/Context;Ljava/lang/String;)V

    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->getMainSharedObject()Ljava/lang/String;

    move-result-object v1

    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->getMainFunction()Ljava/lang/String;

    move-result-object v3

    invoke-static {v1, v3, v0}, Lorg/libsdl/app/SDLActivity;->nativeRunMain(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Object;)I

    move-result v0
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    goto :goto_1

    :catchall_0
    move-exception v0

    new-instance v1, Ljava/lang/StringBuilder;

    invoke-direct {v1}, Ljava/lang/StringBuilder;-><init>()V

    const-string v3, "Erro ao executar etapa nativa: "

    invoke-virtual {v1, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v1

    invoke-virtual {v1, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/Object;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    iput-object v0, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->importError:Ljava/lang/String;

    iget-object v0, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->importError:Ljava/lang/String;

    goto :goto_0

    :cond_0
    const-string v0, "Etapa nativa NAO iniciada devido ao erro de acesso acima."

    :goto_0
    invoke-static {p0, v0}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->note(Landroid/content/Context;Ljava/lang/String;)V

    move v0, v2

    :goto_1
    new-instance v1, Ljava/lang/StringBuilder;

    invoke-direct {v1}, Ljava/lang/StringBuilder;-><init>()V

    const-string v3, "Codigo de saida da etapa: "

    invoke-virtual {v1, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v1

    invoke-virtual {v1, v0}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v1

    invoke-virtual {v1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v1

    invoke-static {p0, v1}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->note(Landroid/content/Context;Ljava/lang/String;)V

    new-instance v1, Landroid/content/Intent;

    invoke-direct {v1}, Landroid/content/Intent;-><init>()V

    const-string v3, "exit_code"

    invoke-virtual {v1, v3, v0}, Landroid/content/Intent;->putExtra(Ljava/lang/String;I)Landroid/content/Intent;

    move-result-object v1

    iget-object v3, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->importError:Ljava/lang/String;

    if-eqz v3, :cond_1

    const-string v3, "import_error"

    iget-object v4, p0, Lio/github/vitarecomps/frontend/FrontendStepActivity;->importError:Ljava/lang/String;

    invoke-virtual {v1, v3, v4}, Landroid/content/Intent;->putExtra(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;

    :cond_1
    if-nez v0, :cond_2

    const/4 v0, -0x1

    goto :goto_2

    :cond_2
    add-int/2addr v0, v2

    :goto_2
    invoke-virtual {p0, v0, v1}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->setResult(ILandroid/content/Intent;)V

    return-void
.end method

.method protected onCreate(Landroid/os/Bundle;)V
    .locals 0

    invoke-static {p0}, Lio/github/vitarecomps/frontend/StepProgress;->prepareWindow(Landroid/app/Activity;)V
    invoke-super {p0, p1}, Lio/github/vitarecomps/frontend/GameActivity;->onCreate(Landroid/os/Bundle;)V

    invoke-static {p0}, Lio/github/vitarecomps/frontend/StepProgress;->attach(Landroid/app/Activity;)V

    return-void
.end method

.method protected onDestroy()V
    .locals 4

    .line 10
    invoke-static {p0}, Lio/github/vitarecomps/frontend/StepProgress;->stop(Landroid/app/Activity;)V

    invoke-super {p0}, Lio/github/vitarecomps/frontend/GameActivity;->onDestroy()V

    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/FrontendStepActivity;->isFinishing()Z

    move-result v0

    if-eqz v0, :cond_0

    new-instance v0, Landroid/os/Handler;

    invoke-static {}, Landroid/os/Looper;->getMainLooper()Landroid/os/Looper;

    move-result-object v1

    invoke-direct {v0, v1}, Landroid/os/Handler;-><init>(Landroid/os/Looper;)V

    new-instance v1, Lio/github/vitarecomps/frontend/FrontendStepActivity$$ExternalSyntheticLambda0;

    invoke-direct {v1}, Lio/github/vitarecomps/frontend/FrontendStepActivity$$ExternalSyntheticLambda0;-><init>()V

    const-wide/16 v2, 0x12c

    invoke-virtual {v0, v1, v2, v3}, Landroid/os/Handler;->postDelayed(Ljava/lang/Runnable;J)Z

    :cond_0
    return-void
.end method
