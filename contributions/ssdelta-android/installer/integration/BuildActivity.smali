.class public Lio/github/vitarecomps/frontend/BuildActivity;
.super Landroid/app/Activity;
.source "BuildActivity.java"


# static fields
.field private static final BUILD:I = 0x6

.field private static final CHECK:I = 0x4

.field private static final DUMP:I = 0x5

.field static final EXTRA_ADD_ADDON:Ljava/lang/String; = "add_addon"

.field private static final FIRMWARE:I = 0x1

.field private static final INSTALL_ADDON:I = 0x7

.field private static final INSTALL_FIRMWARE:I = 0x2

.field private static final INSTALL_GAME:I = 0x3

.field private static final KEY_ADDON:Ljava/lang/String; = "addon"

.field private static final KEY_PARTS:Ljava/lang/String; = "parts"

.field private static final KEY_STEP:Ljava/lang/String; = "step"

.field private static final PICK:I = 0x0

.field private static final PREFS:Ljava/lang/String; = "build"

.field private static final REQUEST_PICK_ADDON:I = 0x4

.field private static final REQUEST_PICK_ADDON_LICENSE:I = 0x5

.field private static final REQUEST_PICK_LICENSE:I = 0x2

.field private static final REQUEST_PICK_PART:I = 0x1

.field private static final REQUEST_STEP:I = 0x3

.field private static final STEP_NAMES:[Ljava/lang/String;

.field private static final TAG:Ljava/lang/String; = "BuildActivity"

.field static volatile visible:Z


# instance fields
.field private addButton:Landroid/widget/Button;

.field private addon:Lorg/json/JSONObject;

.field private addonButton:Landroid/widget/Button;

.field private buildButton:Landroid/widget/Button;

.field private clearButton:Landroid/widget/Button;

.field private errorView:Landroid/widget/TextView;

.field private final formViews:Ljava/util/List;
    .annotation system Ldalvik/annotation/Signature;
        value = {
            "Ljava/util/List<",
            "Landroid/view/View;",
            ">;"
        }
    .end annotation
.end field

.field private game:Lorg/json/JSONObject;

.field private licenseButton:Landroid/widget/Button;

.field private logScroll:Landroid/widget/ScrollView;

.field private logView:Landroid/widget/TextView;

.field private final main:Landroid/os/Handler;

.field private notice:Ljava/lang/String;

.field private parts:Lorg/json/JSONArray;

.field private partsView:Landroid/widget/TextView;

.field private playButton:Landroid/widget/Button;

.field private final poll:Ljava/lang/Runnable;

.field private rebuild:Z

.field private retryButton:Landroid/widget/Button;

.field private serviceGeneration:J

.field private startOverButton:Landroid/widget/Button;

.field private step:I

.field private stepView:Landroid/widget/TextView;

.field private waitingForStep:Z


# direct methods
.method public static synthetic $r8$lambda$-Gg4RIV4XweuJV1_A4Ks0kvYQTI(Lio/github/vitarecomps/frontend/BuildActivity;Landroid/view/View;)V
    .locals 0

    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$onCreate$2(Landroid/view/View;)V

    return-void
.end method

.method public static synthetic $r8$lambda$4EyGWwsMzvD-CUHXxWGgNZXLUUg(Lio/github/vitarecomps/frontend/BuildActivity;Landroid/view/View;)V
    .locals 0

    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$onCreate$7(Landroid/view/View;)V

    return-void
.end method

.method public static synthetic $r8$lambda$Bb7tzNF21NqYMkXlzLgmD_GzBWs(Lio/github/vitarecomps/frontend/BuildActivity;Landroid/view/View;)V
    .locals 0

    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$onCreate$8(Landroid/view/View;)V

    return-void
.end method

.method public static synthetic $r8$lambda$CR1k-4qFgG1k8ApdAc9xYb7To_0(Lio/github/vitarecomps/frontend/BuildActivity;)V
    .locals 0

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$refresh$10()V

    return-void
.end method

.method public static synthetic $r8$lambda$Z55C9iYM3tZRp_cNZYlcxPncJSk(Lio/github/vitarecomps/frontend/BuildActivity;Landroid/view/View;)V
    .locals 0

    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$onCreate$1(Landroid/view/View;)V

    return-void
.end method

.method public static synthetic $r8$lambda$aoAM4N20s2knCd-0BfOz3uMKb4g(Lio/github/vitarecomps/frontend/BuildActivity;Z)V
    .locals 0

    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$onCreate$9(Z)V

    return-void
.end method

.method public static synthetic $r8$lambda$dI0X8aNPGspp-RV3tX9MRo5FGuw(Lio/github/vitarecomps/frontend/BuildActivity;Landroid/view/View;)V
    .locals 0

    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$onCreate$5(Landroid/view/View;)V

    return-void
.end method

.method public static synthetic $r8$lambda$oQ7uFmO2LKP6sHvBAq-B86HZueA(Lio/github/vitarecomps/frontend/BuildActivity;Landroid/view/View;)V
    .locals 0

    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$onCreate$3(Landroid/view/View;)V

    return-void
.end method

.method public static synthetic $r8$lambda$oqYDZq0YA7JW2kp9s7xRpDXMMUc(Lio/github/vitarecomps/frontend/BuildActivity;Landroid/view/View;)V
    .locals 0

    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$onCreate$4(Landroid/view/View;)V

    return-void
.end method

.method public static synthetic $r8$lambda$tnD7MRIxcBMZjtC36PSTSrMoRbY(Lio/github/vitarecomps/frontend/BuildActivity;Landroid/view/View;)V
    .locals 0

    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->lambda$onCreate$6(Landroid/view/View;)V

    return-void
.end method

.method static bridge synthetic -$$Nest$fgetmain(Lio/github/vitarecomps/frontend/BuildActivity;)Landroid/os/Handler;
    .locals 0

    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->main:Landroid/os/Handler;

    return-object p0
.end method

.method static bridge synthetic -$$Nest$monServiceProgress(Lio/github/vitarecomps/frontend/BuildActivity;)V
    .locals 0

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->onServiceProgress()V

    return-void
.end method

.method static constructor <clinit>()V
    .locals 4

    const/16 v0, 0x8

    .line 84
    new-array v0, v0, [Ljava/lang/String;

    const-string v1, ""

    const/4 v2, 0x0

    aput-object v1, v0, v2

    const/4 v1, 0x1

    const-string v3, "Downloading the firmware"

    aput-object v3, v0, v1

    const/4 v1, 0x2

    const-string v3, "Installing the firmware"

    aput-object v3, v0, v1

    const/4 v1, 0x3

    const-string v3, "Installing the game"

    aput-object v3, v0, v1

    const/4 v1, 0x4

    const-string v3, "Checking the game\'s version"

    aput-object v3, v0, v1

    const/4 v1, 0x5

    const-string v3, "Running the game to find its code"

    aput-object v3, v0, v1

    const/4 v1, 0x6

    const-string v3, "Building the native code"

    aput-object v3, v0, v1

    const/4 v1, 0x7

    const-string v3, "Installing the add-on"

    aput-object v3, v0, v1

    sput-object v0, Lio/github/vitarecomps/frontend/BuildActivity;->STEP_NAMES:[Ljava/lang/String;

    .line 88
    sput-boolean v2, Lio/github/vitarecomps/frontend/BuildActivity;->visible:Z

    return-void
.end method

.method public constructor <init>()V
    .locals 2

    .line 74
    invoke-direct {p0}, Landroid/app/Activity;-><init>()V

    .line 90
    new-instance v0, Landroid/os/Handler;

    invoke-static {}, Landroid/os/Looper;->getMainLooper()Landroid/os/Looper;

    move-result-object v1

    invoke-direct {v0, v1}, Landroid/os/Handler;-><init>(Landroid/os/Looper;)V

    iput-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->main:Landroid/os/Handler;

    .line 92
    new-instance v0, Ljava/util/ArrayList;

    invoke-direct {v0}, Ljava/util/ArrayList;-><init>()V

    iput-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->formViews:Ljava/util/List;

    const-wide/16 v0, -0x1

    .line 99
    iput-wide v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->serviceGeneration:J

    const/4 v0, 0x0

    .line 102
    iput-boolean v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->waitingForStep:Z

    .line 293
    new-instance v0, Lio/github/vitarecomps/frontend/BuildActivity$1;

    invoke-direct {v0, p0}, Lio/github/vitarecomps/frontend/BuildActivity$1;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    iput-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->poll:Ljava/lang/Runnable;

    return-void
.end method

.method private buildMinutes()I
    .locals 2

    .line 601
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->game()Lorg/json/JSONObject;

    move-result-object p0

    const-string v0, "android_build_minutes"

    const/16 v1, 0xf

    invoke-virtual {p0, v0, v1}, Lorg/json/JSONObject;->optInt(Ljava/lang/String;I)I

    move-result p0

    return p0
.end method

.method private button(Ljava/lang/String;Landroid/view/View$OnClickListener;)Landroid/widget/Button;
    .locals 1

    .line 315
    new-instance v0, Landroid/widget/Button;

    invoke-direct {v0, p0}, Landroid/widget/Button;-><init>(Landroid/content/Context;)V

    .line 316
    invoke-virtual {v0, p1}, Landroid/widget/Button;->setText(Ljava/lang/CharSequence;)V

    const/4 p1, 0x0

    .line 317
    invoke-virtual {v0, p1}, Landroid/widget/Button;->setAllCaps(Z)V

    .line 318
    invoke-virtual {v0, p2}, Landroid/widget/Button;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 319
    new-instance p1, Landroid/widget/LinearLayout$LayoutParams;

    const/4 p2, -0x2

    invoke-direct {p1, p2, p2}, Landroid/widget/LinearLayout$LayoutParams;-><init>(II)V

    const/16 p2, 0x8

    .line 320
    invoke-direct {p0, p2}, Lio/github/vitarecomps/frontend/BuildActivity;->dp(I)I

    move-result p0

    invoke-virtual {p1, p0}, Landroid/widget/LinearLayout$LayoutParams;->setMarginEnd(I)V

    .line 321
    invoke-virtual {v0, p1}, Landroid/widget/Button;->setLayoutParams(Landroid/view/ViewGroup$LayoutParams;)V

    return-object v0
.end method

.method private canRebuild()Z
    .locals 8

    .line 611
    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->gameDir(Landroid/content/Context;)Ljava/io/File;

    move-result-object v0

    .line 612
    new-instance v1, Ljava/util/ArrayList;

    invoke-direct {v1}, Ljava/util/ArrayList;-><init>()V

    .line 613
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->game()Lorg/json/JSONObject;

    move-result-object v2

    const-string v3, "title_id"

    invoke-virtual {v2, v3}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v2

    invoke-interface {v1, v2}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 614
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->game()Lorg/json/JSONObject;

    move-result-object v2

    const-string v3, "other_title_ids"

    invoke-virtual {v2, v3}, Lorg/json/JSONObject;->optJSONArray(Ljava/lang/String;)Lorg/json/JSONArray;

    move-result-object v2

    const/4 v3, 0x0

    move v4, v3

    :goto_0
    if-eqz v2, :cond_0

    .line 615
    invoke-virtual {v2}, Lorg/json/JSONArray;->length()I

    move-result v5

    if-ge v4, v5, :cond_0

    .line 616
    invoke-virtual {v2, v4}, Lorg/json/JSONArray;->optString(I)Ljava/lang/String;

    move-result-object v5

    invoke-interface {v1, v5}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    add-int/lit8 v4, v4, 0x1

    goto :goto_0

    .line 617
    :cond_0
    invoke-interface {v1}, Ljava/util/List;->iterator()Ljava/util/Iterator;

    move-result-object v1

    :cond_1
    :goto_1
    invoke-interface {v1}, Ljava/util/Iterator;->hasNext()Z

    move-result v2

    if-eqz v2, :cond_3

    invoke-interface {v1}, Ljava/util/Iterator;->next()Ljava/lang/Object;

    move-result-object v2

    check-cast v2, Ljava/lang/String;

    .line 618
    invoke-virtual {v2}, Ljava/lang/String;->isEmpty()Z

    move-result v4

    if-nez v4, :cond_1

    new-instance v4, Ljava/io/File;

    new-instance v5, Ljava/lang/StringBuilder;

    const-string v6, "fs/ux0/app/"

    invoke-direct {v5, v6}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {v5, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v5

    const-string v6, "/eboot.bin"

    invoke-virtual {v5, v6}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v5

    invoke-virtual {v5}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v5

    invoke-direct {v4, v0, v5}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-virtual {v4}, Ljava/io/File;->isFile()Z

    move-result v4

    if-nez v4, :cond_2

    goto :goto_1

    .line 620
    :cond_2
    new-instance v4, Ljava/io/File;

    new-instance v5, Ljava/lang/StringBuilder;

    invoke-direct {v5}, Ljava/lang/StringBuilder;-><init>()V

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->game()Lorg/json/JSONObject;

    move-result-object v6

    const-string v7, "id"

    invoke-virtual {v6, v7}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v6

    invoke-virtual {v5, v6}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v5

    const-string v6, "/elfdumps/"

    invoke-virtual {v5, v6}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v5

    invoke-virtual {v5, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v2

    invoke-direct {v4, v0, v2}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    new-instance v2, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda11;

    invoke-direct {v2}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda11;-><init>()V

    invoke-virtual {v4, v2}, Ljava/io/File;->list(Ljava/io/FilenameFilter;)[Ljava/lang/String;

    move-result-object v2

    if-eqz v2, :cond_1

    .line 621
    array-length v2, v2

    if-lez v2, :cond_1

    const/4 p0, 0x1

    return p0

    :cond_3
    return v3
.end method

.method private displayName(Landroid/net/Uri;)Ljava/lang/String;
    .locals 7

    .line 416
    :try_start_0
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->getContentResolver()Landroid/content/ContentResolver;

    move-result-object v0

    const/4 p0, 0x1

    new-array v2, p0, [Ljava/lang/String;

    const-string p0, "_display_name"

    const/4 v6, 0x0

    aput-object p0, v2, v6
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_1

    const/4 v4, 0x0

    const/4 v5, 0x0

    const/4 v3, 0x0

    move-object v1, p1

    :try_start_1
    invoke-virtual/range {v0 .. v5}, Landroid/content/ContentResolver;->query(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;

    move-result-object p0
    :try_end_1
    .catch Ljava/lang/Exception; {:try_start_1 .. :try_end_1} :catch_0

    if-eqz p0, :cond_2

    .line 417
    :try_start_2
    invoke-interface {p0}, Landroid/database/Cursor;->moveToFirst()Z

    move-result p1

    if-eqz p1, :cond_2

    invoke-interface {p0, v6}, Landroid/database/Cursor;->getString(I)Ljava/lang/String;

    move-result-object p1

    if-eqz p1, :cond_2

    .line 418
    invoke-interface {p0, v6}, Landroid/database/Cursor;->getString(I)Ljava/lang/String;

    move-result-object p1
    :try_end_2
    .catchall {:try_start_2 .. :try_end_2} :catchall_0

    if-eqz p0, :cond_0

    .line 419
    :try_start_3
    invoke-interface {p0}, Landroid/database/Cursor;->close()V
    :try_end_3
    .catch Ljava/lang/Exception; {:try_start_3 .. :try_end_3} :catch_0

    :cond_0
    return-object p1

    :catchall_0
    move-exception v0

    move-object p1, v0

    if-eqz p0, :cond_1

    .line 416
    :try_start_4
    invoke-interface {p0}, Landroid/database/Cursor;->close()V
    :try_end_4
    .catchall {:try_start_4 .. :try_end_4} :catchall_1

    goto :goto_0

    :catchall_1
    move-exception v0

    move-object p0, v0

    :try_start_5
    invoke-virtual {p1, p0}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_1
    :goto_0
    throw p1

    :cond_2
    if-eqz p0, :cond_3

    .line 419
    invoke-interface {p0}, Landroid/database/Cursor;->close()V
    :try_end_5
    .catch Ljava/lang/Exception; {:try_start_5 .. :try_end_5} :catch_0

    goto :goto_2

    :catch_0
    move-exception v0

    goto :goto_1

    :catch_1
    move-exception v0

    move-object v1, p1

    :goto_1
    move-object p0, v0

    .line 420
    const-string p1, "BuildActivity"

    const-string v0, "name"

    invoke-static {p1, v0, p0}, Landroid/util/Log;->w(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Throwable;)I

    .line 422
    :cond_3
    :goto_2
    invoke-virtual {v1}, Landroid/net/Uri;->getLastPathSegment()Ljava/lang/String;

    move-result-object p0

    return-object p0
.end method

.method private dp(I)I
    .locals 0

    int-to-float p1, p1

    .line 302
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->getResources()Landroid/content/res/Resources;

    move-result-object p0

    invoke-virtual {p0}, Landroid/content/res/Resources;->getDisplayMetrics()Landroid/util/DisplayMetrics;

    move-result-object p0

    iget p0, p0, Landroid/util/DisplayMetrics;->density:F

    mul-float/2addr p1, p0

    invoke-static {p1}, Ljava/lang/Math;->round(F)I

    move-result p0

    return p0
.end method

.method private dumpSeconds()I
    .locals 5

    const/16 v0, 0x2d

    .line 628
    :try_start_0
    new-instance v1, Ljava/io/FileInputStream;

    new-instance v2, Ljava/io/File;

    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildTools;->root(Landroid/content/Context;)Ljava/io/File;

    move-result-object p0

    const-string v3, "game.json"

    invoke-direct {v2, p0, v3}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-direct {v1, v2}, Ljava/io/FileInputStream;-><init>(Ljava/io/File;)V
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    .line 629
    :try_start_1
    new-instance p0, Lorg/json/JSONObject;

    new-instance v2, Ljava/lang/String;

    invoke-static {v1}, Lio/github/vitarecomps/frontend/BuildActivity;->readAll(Ljava/io/InputStream;)[B

    move-result-object v3

    sget-object v4, Ljava/nio/charset/StandardCharsets;->UTF_8:Ljava/nio/charset/Charset;

    invoke-direct {v2, v3, v4}, Ljava/lang/String;-><init>([BLjava/nio/charset/Charset;)V

    invoke-direct {p0, v2}, Lorg/json/JSONObject;-><init>(Ljava/lang/String;)V

    const-string v2, "dump_seconds"

    invoke-virtual {p0, v2, v0}, Lorg/json/JSONObject;->optInt(Ljava/lang/String;I)I

    move-result p0
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    .line 630
    :try_start_2
    invoke-virtual {v1}, Ljava/io/InputStream;->close()V
    :try_end_2
    .catch Ljava/lang/Exception; {:try_start_2 .. :try_end_2} :catch_0

    return p0

    :catchall_0
    move-exception p0

    .line 628
    :try_start_3
    invoke-virtual {v1}, Ljava/io/InputStream;->close()V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_1

    goto :goto_0

    :catchall_1
    move-exception v1

    :try_start_4
    invoke-virtual {p0, v1}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_0
    throw p0
    :try_end_4
    .catch Ljava/lang/Exception; {:try_start_4 .. :try_end_4} :catch_0

    :catch_0
    return v0
.end method

.method private frontend([Ljava/lang/String;)V
    .locals 2

    invoke-static {p0, p1}, Lio/github/vitarecomps/frontend/MainInstallProgress;->begin(Landroid/app/Activity;[Ljava/lang/String;)V

    .line 645
    new-instance v0, Ljava/lang/StringBuilder;

    const-string v1, "$ front end "

    invoke-direct {v0, v1}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    const-string v1, " "

    invoke-static {v1, p1}, Ljava/lang/String;->join(Ljava/lang/CharSequence;[Ljava/lang/CharSequence;)Ljava/lang/String;

    move-result-object v1

    invoke-virtual {v0, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    invoke-static {p0, v0}, Lio/github/vitarecomps/frontend/BuildService;->append(Landroid/content/Context;Ljava/lang/String;)V

    const/4 v0, 0x1

    .line 646
    iput-boolean v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->waitingForStep:Z

    .line 647
    new-instance v0, Landroid/content/Intent;

    const-class v1, Lio/github/vitarecomps/frontend/FrontendStepActivity;

    invoke-direct {v0, p0, v1}, Landroid/content/Intent;-><init>(Landroid/content/Context;Ljava/lang/Class;)V

    const-string v1, "args"

    invoke-virtual {v0, v1, p1}, Landroid/content/Intent;->putExtra(Ljava/lang/String;[Ljava/lang/String;)Landroid/content/Intent;

    move-result-object p1

    const/4 v0, 0x3

    invoke-virtual {p0, p1, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->startActivityForResult(Landroid/content/Intent;I)V

    return-void
.end method

.method private game()Lorg/json/JSONObject;
    .locals 6

    .line 584
    const-string v0, "game.json"

    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->game:Lorg/json/JSONObject;

    if-eqz v1, :cond_0

    return-object v1

    .line 586
    :cond_0
    new-instance v1, Lorg/json/JSONObject;

    invoke-direct {v1}, Lorg/json/JSONObject;-><init>()V

    iput-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->game:Lorg/json/JSONObject;

    .line 587
    :try_start_0
    new-instance v1, Ljava/util/zip/ZipInputStream;

    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v2

    const-string v3, "buildtools.zip"

    invoke-virtual {v2, v3}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;

    move-result-object v2

    invoke-direct {v1, v2}, Ljava/util/zip/ZipInputStream;-><init>(Ljava/io/InputStream;)V
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    .line 588
    :cond_1
    :try_start_1
    invoke-virtual {v1}, Ljava/util/zip/ZipInputStream;->getNextEntry()Ljava/util/zip/ZipEntry;

    move-result-object v2

    if-eqz v2, :cond_2

    .line 589
    invoke-virtual {v2}, Ljava/util/zip/ZipEntry;->getName()Ljava/lang/String;

    move-result-object v2

    invoke-virtual {v2, v0}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result v2

    if-eqz v2, :cond_1

    .line 590
    new-instance v2, Lorg/json/JSONObject;

    new-instance v3, Ljava/lang/String;

    invoke-static {v1}, Lio/github/vitarecomps/frontend/BuildActivity;->readAll(Ljava/io/InputStream;)[B

    move-result-object v4

    sget-object v5, Ljava/nio/charset/StandardCharsets;->UTF_8:Ljava/nio/charset/Charset;

    invoke-direct {v3, v4, v5}, Ljava/lang/String;-><init>([BLjava/nio/charset/Charset;)V

    invoke-direct {v2, v3}, Lorg/json/JSONObject;-><init>(Ljava/lang/String;)V

    iput-object v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->game:Lorg/json/JSONObject;
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    .line 593
    :cond_2
    :try_start_2
    invoke-virtual {v1}, Ljava/util/zip/ZipInputStream;->close()V
    :try_end_2
    .catch Ljava/lang/Exception; {:try_start_2 .. :try_end_2} :catch_0

    goto :goto_1

    :catchall_0
    move-exception v2

    .line 587
    :try_start_3
    invoke-virtual {v1}, Ljava/util/zip/ZipInputStream;->close()V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_1

    goto :goto_0

    :catchall_1
    move-exception v1

    :try_start_4
    invoke-virtual {v2, v1}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_0
    throw v2
    :try_end_4
    .catch Ljava/lang/Exception; {:try_start_4 .. :try_end_4} :catch_0

    :catch_0
    move-exception v1

    .line 594
    const-string v2, "BuildActivity"

    invoke-static {v2, v0, v1}, Landroid/util/Log;->w(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Throwable;)I

    .line 596
    :goto_1
    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->game:Lorg/json/JSONObject;

    return-object p0
.end method

.method static gameDir(Landroid/content/Context;)Ljava/io/File;
    .locals 1

    const/4 v0, 0x0

    .line 107
    invoke-virtual {p0, v0}, Landroid/content/Context;->getExternalFilesDir(Ljava/lang/String;)Ljava/io/File;

    move-result-object v0

    if-eqz v0, :cond_0

    return-object v0

    .line 108
    :cond_0
    invoke-virtual {p0}, Landroid/content/Context;->getFilesDir()Ljava/io/File;

    move-result-object p0

    return-object p0
.end method

.method private gameName()Ljava/lang/String;
    .locals 2

    .line 605
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->game()Lorg/json/JSONObject;

    move-result-object v0

    const-string v1, "name"

    invoke-virtual {v0, v1}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v0

    .line 606
    invoke-virtual {v0}, Ljava/lang/String;->isEmpty()Z

    move-result v1

    if-nez v1, :cond_0

    return-object v0

    :cond_0
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->getApplicationInfo()Landroid/content/pm/ApplicationInfo;

    move-result-object v0

    iget v0, v0, Landroid/content/pm/ApplicationInfo;->labelRes:I

    invoke-virtual {p0, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->getString(I)Ljava/lang/String;

    move-result-object p0

    return-object p0
.end method

.method static synthetic lambda$canRebuild$11(Ljava/io/File;Ljava/lang/String;)Z
    .locals 0

    .line 620
    const-string p0, "_eboot.elf"

    invoke-virtual {p1, p0}, Ljava/lang/String;->endsWith(Ljava/lang/String;)Z

    move-result p0

    return p0
.end method

.method static synthetic lambda$needsSetup$0(Ljava/io/File;Ljava/lang/String;)Z
    .locals 0

    .line 144
    const-string p0, "ssd_aot_"

    invoke-virtual {p1, p0}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z

    move-result p0

    if-eqz p0, :cond_0

    const-string p0, ".so"

    invoke-virtual {p1, p0}, Ljava/lang/String;->endsWith(Ljava/lang/String;)Z

    move-result p0

    if-eqz p0, :cond_0

    const/4 p0, 0x1

    return p0

    :cond_0
    const/4 p0, 0x0

    return p0
.end method

.method private synthetic lambda$onCreate$1(Landroid/view/View;)V
    .locals 0

    const/4 p1, 0x1

    .line 202
    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->pick(I)V

    return-void
.end method

.method private synthetic lambda$onCreate$2(Landroid/view/View;)V
    .locals 0

    const/4 p1, 0x2

    .line 203
    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->pick(I)V

    return-void
.end method

.method private synthetic lambda$onCreate$3(Landroid/view/View;)V
    .locals 0

    .line 205
    new-instance p1, Lorg/json/JSONArray;

    invoke-direct {p1}, Lorg/json/JSONArray;-><init>()V

    iput-object p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    .line 206
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    .line 207
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->refresh()V

    return-void
.end method

.method private synthetic lambda$onCreate$4(Landroid/view/View;)V
    .locals 0

    .line 221
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->startSetup()V

    return-void
.end method

.method private synthetic lambda$onCreate$5(Landroid/view/View;)V
    .locals 1

    .line 223
    sget-object p1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    const/4 v0, 0x0

    iput-object v0, p1, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    .line 224
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->run()V

    return-void
.end method

.method private synthetic lambda$onCreate$6(Landroid/view/View;)V
    .locals 0

    .line 226
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->play()V

    return-void
.end method

.method private synthetic lambda$onCreate$7(Landroid/view/View;)V
    .locals 2

    const/4 p1, 0x0

    .line 228
    iput p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    .line 229
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    .line 230
    sget-object p1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    monitor-enter p1

    .line 231
    :try_start_0
    sget-object v0, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    const/4 v1, 0x0

    iput-object v1, v0, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    .line 232
    monitor-exit p1
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    .line 233
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->refresh()V

    return-void

    :catchall_0
    move-exception p0

    .line 232
    :try_start_1
    monitor-exit p1
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    throw p0
.end method

.method private synthetic lambda$onCreate$8(Landroid/view/View;)V
    .locals 0

    const/4 p1, 0x4

    .line 235
    invoke-direct {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->pick(I)V

    return-void
.end method

.method private synthetic lambda$onCreate$9(Z)V
    .locals 0

    if-eqz p1, :cond_0

    .line 276
    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addonButton:Landroid/widget/Button;

    goto :goto_0

    :cond_0
    iget-object p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {p1}, Lorg/json/JSONArray;->length()I

    move-result p1

    if-nez p1, :cond_1

    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addButton:Landroid/widget/Button;

    goto :goto_0

    :cond_1
    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->buildButton:Landroid/widget/Button;

    :goto_0
    invoke-virtual {p0}, Landroid/widget/Button;->requestFocus()Z

    return-void
.end method

.method private synthetic lambda$refresh$10()V
    .locals 1

    .line 380
    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logScroll:Landroid/widget/ScrollView;

    const/16 v0, 0x82

    invoke-virtual {p0, v0}, Landroid/widget/ScrollView;->fullScroll(I)Z

    return-void
.end method

.method static needsSetup(Landroid/content/Context;)Z
    .locals 10

    .line 134
    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildTools;->present(Landroid/content/Context;)Z

    move-result v0

    const/4 v1, 0x0

    if-nez v0, :cond_0

    return v1

    .line 136
    :cond_0
    const-string v0, "build"

    invoke-virtual {p0, v0, v1}, Landroid/content/Context;->getSharedPreferences(Ljava/lang/String;I)Landroid/content/SharedPreferences;

    move-result-object v0

    const-string v2, "step"

    invoke-interface {v0, v2, v1}, Landroid/content/SharedPreferences;->getInt(Ljava/lang/String;I)I

    move-result v0

    const/4 v2, 0x1

    if-eqz v0, :cond_1

    return v2

    .line 138
    :cond_1
    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->gameDir(Landroid/content/Context;)Ljava/io/File;

    move-result-object v0

    .line 140
    new-instance v3, Ljava/io/File;

    const-string v4, "fs/ux0/app"

    invoke-direct {v3, v0, v4}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-virtual {v3}, Ljava/io/File;->listFiles()[Ljava/io/File;

    move-result-object v3

    if-eqz v3, :cond_2

    .line 142
    array-length v4, v3

    move v5, v1

    move v6, v5

    :goto_0
    if-ge v5, v4, :cond_3

    aget-object v7, v3, v5

    .line 143
    new-instance v8, Ljava/io/File;

    const-string v9, "eboot.bin"

    invoke-direct {v8, v7, v9}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-virtual {v8}, Ljava/io/File;->isFile()Z

    move-result v7

    or-int/2addr v6, v7

    add-int/lit8 v5, v5, 0x1

    goto :goto_0

    :cond_2
    move v6, v1

    .line 144
    :cond_3
    new-instance v3, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda2;

    invoke-direct {v3}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda2;-><init>()V

    invoke-virtual {v0, v3}, Ljava/io/File;->list(Ljava/io/FilenameFilter;)[Ljava/lang/String;

    move-result-object v3

    if-eqz v6, :cond_6

    if-eqz v3, :cond_6

    .line 145
    array-length v3, v3

    if-nez v3, :cond_4

    goto :goto_2

    .line 147
    :cond_4
    new-instance v3, Ljava/io/File;

    const-string v4, "android-build.json"

    invoke-direct {v3, v0, v4}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    .line 148
    invoke-virtual {v3}, Ljava/io/File;->isFile()Z

    move-result v0

    if-nez v0, :cond_5

    return v1

    .line 150
    :cond_5
    :try_start_0
    new-instance v0, Ljava/io/FileInputStream;

    invoke-direct {v0, v3}, Ljava/io/FileInputStream;-><init>(Ljava/io/File;)V
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    .line 151
    :try_start_1
    new-instance v1, Lorg/json/JSONObject;

    new-instance v3, Ljava/lang/String;

    invoke-static {v0}, Lio/github/vitarecomps/frontend/BuildActivity;->readAll(Ljava/io/InputStream;)[B

    move-result-object v4

    sget-object v5, Ljava/nio/charset/StandardCharsets;->UTF_8:Ljava/nio/charset/Charset;

    invoke-direct {v3, v4, v5}, Ljava/lang/String;-><init>([BLjava/nio/charset/Charset;)V

    invoke-direct {v1, v3}, Lorg/json/JSONObject;-><init>(Ljava/lang/String;)V

    const-string v3, "stamp"

    invoke-virtual {v1, v3}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v1

    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->stamp(Landroid/content/Context;)Ljava/lang/String;

    move-result-object p0

    invoke-virtual {v1, p0}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result p0
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    xor-int/2addr p0, v2

    .line 152
    :try_start_2
    invoke-virtual {v0}, Ljava/io/InputStream;->close()V
    :try_end_2
    .catch Ljava/lang/Exception; {:try_start_2 .. :try_end_2} :catch_0

    return p0

    :catchall_0
    move-exception p0

    .line 150
    :try_start_3
    invoke-virtual {v0}, Ljava/io/InputStream;->close()V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_1

    goto :goto_1

    :catchall_1
    move-exception v0

    :try_start_4
    invoke-virtual {p0, v0}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :goto_1
    throw p0
    :try_end_4
    .catch Ljava/lang/Exception; {:try_start_4 .. :try_end_4} :catch_0

    :catch_0
    :cond_6
    :goto_2
    return v2
.end method

.method private onServiceProgress()V
    .locals 10

    .line 654
    sget-object v0, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    monitor-enter v0

    .line 655
    :try_start_0
    iget-wide v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->serviceGeneration:J

    const-wide/16 v3, 0x0

    cmp-long v1, v1, v3

    const/4 v2, 0x0

    const/4 v5, 0x1

    if-ltz v1, :cond_0

    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-wide v6, v1, Lio/github/vitarecomps/frontend/BuildService$State;->generation:J

    iget-wide v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->serviceGeneration:J

    cmp-long v1, v6, v8

    if-ltz v1, :cond_0

    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-boolean v1, v1, Lio/github/vitarecomps/frontend/BuildService$State;->running:Z

    if-nez v1, :cond_0

    move v1, v5

    goto :goto_0

    :cond_0
    move v1, v2

    .line 656
    :goto_0
    sget-object v6, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget v6, v6, Lio/github/vitarecomps/frontend/BuildService$State;->exitCode:I

    .line 657
    monitor-exit v0
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    const/4 v0, 0x6

    if-eqz v1, :cond_3

    const-wide/16 v3, -0x1

    .line 659
    iput-wide v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->serviceGeneration:J

    if-nez v6, :cond_5

    .line 661
    iget v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    if-ne v1, v0, :cond_1

    goto :goto_1

    :cond_1
    add-int/lit8 v2, v1, 0x1

    :goto_1
    iput v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    if-nez v2, :cond_2

    .line 663
    new-instance v0, Lorg/json/JSONArray;

    invoke-direct {v0}, Lorg/json/JSONArray;-><init>()V

    iput-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    .line 664
    :cond_2
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    .line 665
    iget v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    if-eqz v0, :cond_5

    .line 666
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->run()V

    goto :goto_2

    .line 668
    :cond_3
    iget-wide v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->serviceGeneration:J

    cmp-long v1, v1, v3

    if-gez v1, :cond_5

    iget v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    if-eqz v1, :cond_5

    iget-boolean v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->waitingForStep:Z

    if-nez v1, :cond_5

    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-boolean v1, v1, Lio/github/vitarecomps/frontend/BuildService$State;->running:Z

    if-nez v1, :cond_5

    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-object v1, v1, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    if-nez v1, :cond_5

    iget v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    if-eq v1, v5, :cond_4

    const/4 v2, 0x4

    if-eq v1, v2, :cond_4

    if-ne v1, v0, :cond_5

    .line 671
    :cond_4
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->run()V

    .line 673
    :cond_5
    :goto_2
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->refresh()V

    return-void

    :catchall_0
    move-exception p0

    .line 657
    :try_start_1
    monitor-exit v0
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    throw p0
.end method

.method private pick(I)V
    .locals 2

    .line 409
    new-instance v0, Landroid/content/Intent;

    const-string v1, "android.intent.action.OPEN_DOCUMENT"

    invoke-direct {v0, v1}, Landroid/content/Intent;-><init>(Ljava/lang/String;)V

    .line 410
    const-string v1, "android.intent.category.OPENABLE"

    invoke-virtual {v0, v1}, Landroid/content/Intent;->addCategory(Ljava/lang/String;)Landroid/content/Intent;

    .line 411
    const-string v1, "*/*"

    invoke-virtual {v0, v1}, Landroid/content/Intent;->setType(Ljava/lang/String;)Landroid/content/Intent;

    .line 412
    invoke-virtual {p0, v0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->startActivityForResult(Landroid/content/Intent;I)V

    return-void
.end method

.method private play()V
    .locals 2

    .line 677
    new-instance v0, Landroid/content/Intent;

    const-class v1, Lio/github/vitarecomps/frontend/SetupActivity;

    invoke-direct {v0, p0, v1}, Landroid/content/Intent;-><init>(Landroid/content/Context;Ljava/lang/Class;)V

    invoke-virtual {p0, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->startActivity(Landroid/content/Intent;)V

    .line 678
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->finish()V

    return-void
.end method

.method private static readAll(Ljava/io/InputStream;)[B
    .locals 4
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .line 682
    new-instance v0, Ljava/io/ByteArrayOutputStream;

    invoke-direct {v0}, Ljava/io/ByteArrayOutputStream;-><init>()V

    const/16 v1, 0x2000

    .line 683
    new-array v1, v1, [B

    .line 684
    :goto_0
    invoke-virtual {p0, v1}, Ljava/io/InputStream;->read([B)I

    move-result v2

    if-lez v2, :cond_0

    const/4 v3, 0x0

    .line 685
    invoke-virtual {v0, v1, v3, v2}, Ljava/io/ByteArrayOutputStream;->write([BII)V

    goto :goto_0

    .line 686
    :cond_0
    invoke-virtual {v0}, Ljava/io/ByteArrayOutputStream;->toByteArray()[B

    move-result-object p0

    return-object p0
.end method

.method private refresh()V
    .locals 14

    .line 326
    iget v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    const/4 v1, 0x1

    const/4 v2, 0x0

    if-eqz v0, :cond_0

    move v0, v1

    goto :goto_0

    :cond_0
    move v0, v2

    .line 330
    :goto_0
    sget-object v3, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    monitor-enter v3

    .line 331
    :try_start_0
    sget-object v4, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-boolean v4, v4, Lio/github/vitarecomps/frontend/BuildService$State;->running:Z

    .line 332
    sget-object v5, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-object v5, v5, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    .line 333
    sget-object v6, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-object v6, v6, Lio/github/vitarecomps/frontend/BuildService$State;->step:Ljava/lang/String;

    .line 334
    new-instance v7, Ljava/util/ArrayList;

    sget-object v8, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-object v8, v8, Lio/github/vitarecomps/frontend/BuildService$State;->lines:Ljava/util/List;

    invoke-direct {v7, v8}, Ljava/util/ArrayList;-><init>(Ljava/util/Collection;)V

    .line 335
    monitor-exit v3
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    invoke-static {p0, v0, v4, v5}, Lio/github/vitarecomps/frontend/ImportScreen;->setupState(Landroid/app/Activity;ZZLjava/lang/String;)V

    .line 336
    new-instance v3, Ljava/lang/StringBuilder;

    invoke-direct {v3}, Ljava/lang/StringBuilder;-><init>()V

    move v8, v2

    .line 337
    :goto_1
    iget-object v9, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v9}, Lorg/json/JSONArray;->length()I

    move-result v9

    const/16 v10, 0xa

    if-ge v8, v9, :cond_2

    .line 338
    iget-object v9, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v9, v8}, Lorg/json/JSONArray;->optJSONObject(I)Lorg/json/JSONObject;

    move-result-object v9

    add-int/lit8 v8, v8, 0x1

    .line 339
    invoke-virtual {v3, v8}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v11

    const-string v12, ". "

    invoke-virtual {v11, v12}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v11

    const-string v12, "name"

    invoke-virtual {v9, v12}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v12

    invoke-virtual {v11, v12}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    .line 340
    const-string v11, "license_name"

    invoke-virtual {v9, v11}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v11

    invoke-virtual {v11}, Ljava/lang/String;->isEmpty()Z

    move-result v11

    if-nez v11, :cond_1

    .line 341
    const-string v11, "  (licence: "

    invoke-virtual {v3, v11}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v11

    const-string v12, "license_name"

    invoke-virtual {v9, v12}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v9

    invoke-virtual {v11, v9}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v9

    const-string v11, ")"

    invoke-virtual {v9, v11}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    .line 342
    :cond_1
    invoke-virtual {v3, v10}, Ljava/lang/StringBuilder;->append(C)Ljava/lang/StringBuilder;

    goto :goto_1

    .line 344
    :cond_2
    iget v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    const/4 v9, 0x7

    if-ne v8, v9, :cond_4

    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    if-eqz v8, :cond_4

    .line 345
    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->partsView:Landroid/widget/TextView;

    new-instance v8, Ljava/lang/StringBuilder;

    const-string v11, "From:\n"

    invoke-direct {v8, v11}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    iget-object v11, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    const-string v12, "name"

    invoke-virtual {v11, v12}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v11

    invoke-virtual {v8, v11}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v8

    .line 346
    iget-object v11, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    const-string v12, "license_name"

    invoke-virtual {v11, v12}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v11

    invoke-virtual {v11}, Ljava/lang/String;->isEmpty()Z

    move-result v11

    if-eqz v11, :cond_3

    const-string v11, ""

    goto :goto_2

    :cond_3
    new-instance v11, Ljava/lang/StringBuilder;

    const-string v12, "  (licence: "

    invoke-direct {v11, v12}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    iget-object v12, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    const-string v13, "license_name"

    invoke-virtual {v12, v13}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v12

    invoke-virtual {v11, v12}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v11

    const-string v12, ")"

    invoke-virtual {v11, v12}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v11

    invoke-virtual {v11}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v11

    :goto_2
    invoke-virtual {v8, v11}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v8

    invoke-virtual {v8}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v8

    .line 345
    invoke-virtual {v3, v8}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    goto :goto_5

    .line 348
    :cond_4
    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->partsView:Landroid/widget/TextView;

    iget-object v11, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v11}, Lorg/json/JSONArray;->length()I

    move-result v11

    if-nez v11, :cond_5

    const-string v3, "No files chosen yet."

    goto :goto_4

    :cond_5
    new-instance v11, Ljava/lang/StringBuilder;

    invoke-direct {v11}, Ljava/lang/StringBuilder;-><init>()V

    if-eqz v0, :cond_6

    const-string v12, "From:\n"

    goto :goto_3

    :cond_6
    const-string v12, ""

    :goto_3
    invoke-virtual {v11, v12}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v11

    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/String;->trim()Ljava/lang/String;

    move-result-object v3

    invoke-virtual {v11, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v3

    :goto_4
    invoke-virtual {v8, v3}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    .line 349
    :goto_5
    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v3}, Lorg/json/JSONArray;->length()I

    move-result v3

    if-lez v3, :cond_7

    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v3}, Lorg/json/JSONArray;->length()I

    move-result v8

    sub-int/2addr v8, v1

    invoke-virtual {v3, v8}, Lorg/json/JSONArray;->optJSONObject(I)Lorg/json/JSONObject;

    move-result-object v3

    const-string v8, "name"

    invoke-virtual {v3, v8}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/String;->toLowerCase()Ljava/lang/String;

    move-result-object v3

    const-string v8, ".pkg"

    invoke-virtual {v3, v8}, Ljava/lang/String;->endsWith(Ljava/lang/String;)Z

    move-result v3

    if-eqz v3, :cond_7

    move v3, v1

    goto :goto_6

    :cond_7
    move v3, v2

    .line 350
    :goto_6
    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addButton:Landroid/widget/Button;

    xor-int/lit8 v11, v0, 0x1

    invoke-virtual {v8, v11}, Landroid/widget/Button;->setEnabled(Z)V

    .line 351
    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->licenseButton:Landroid/widget/Button;

    if-nez v0, :cond_8

    if-eqz v3, :cond_8

    move v3, v1

    goto :goto_7

    :cond_8
    move v3, v2

    :goto_7
    invoke-virtual {v8, v3}, Landroid/widget/Button;->setEnabled(Z)V

    .line 352
    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->clearButton:Landroid/widget/Button;

    if-nez v0, :cond_9

    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v8}, Lorg/json/JSONArray;->length()I

    move-result v8

    if-lez v8, :cond_9

    move v8, v1

    goto :goto_8

    :cond_9
    move v8, v2

    :goto_8
    invoke-virtual {v3, v8}, Landroid/widget/Button;->setEnabled(Z)V

    .line 353
    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->buildButton:Landroid/widget/Button;

    if-nez v0, :cond_b

    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v8}, Lorg/json/JSONArray;->length()I

    move-result v8

    if-gtz v8, :cond_a

    iget-boolean v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->rebuild:Z

    if-eqz v8, :cond_b

    :cond_a
    move v8, v1

    goto :goto_9

    :cond_b
    move v8, v2

    :goto_9
    invoke-virtual {v3, v8}, Landroid/widget/Button;->setEnabled(Z)V

    if-nez v0, :cond_c

    .line 356
    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->needsSetup(Landroid/content/Context;)Z

    move-result v3

    if-nez v3, :cond_c

    move v3, v1

    goto :goto_a

    :cond_c
    move v3, v2

    .line 357
    :goto_a
    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->formViews:Ljava/util/List;

    invoke-interface {v8}, Ljava/util/List;->iterator()Ljava/util/Iterator;

    move-result-object v8

    :goto_b
    invoke-interface {v8}, Ljava/util/Iterator;->hasNext()Z

    move-result v11

    const/16 v12, 0x8

    if-eqz v11, :cond_f

    invoke-interface {v8}, Ljava/util/Iterator;->next()Ljava/lang/Object;

    move-result-object v11

    check-cast v11, Landroid/view/View;

    if-nez v0, :cond_e

    if-eqz v3, :cond_d

    goto :goto_c

    :cond_d
    move v12, v2

    .line 358
    :cond_e
    :goto_c
    invoke-virtual {v11, v12}, Landroid/view/View;->setVisibility(I)V

    goto :goto_b

    .line 359
    :cond_f
    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->partsView:Landroid/widget/TextView;

    iget v11, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    if-ne v11, v9, :cond_11

    :cond_10
    move v9, v2

    goto :goto_d

    :cond_11
    if-nez v3, :cond_12

    if-eqz v0, :cond_10

    .line 360
    iget-object v9, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v9}, Lorg/json/JSONArray;->length()I

    move-result v9

    if-nez v9, :cond_10

    :cond_12
    move v9, v12

    .line 359
    :goto_d
    invoke-virtual {v8, v9}, Landroid/widget/TextView;->setVisibility(I)V

    .line 361
    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->buildButton:Landroid/widget/Button;

    if-eqz v0, :cond_13

    move v9, v12

    goto :goto_e

    :cond_13
    move v9, v2

    :goto_e
    invoke-virtual {v8, v9}, Landroid/widget/Button;->setVisibility(I)V

    .line 362
    iget-object v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addonButton:Landroid/widget/Button;

    if-eqz v3, :cond_14

    move v3, v2

    goto :goto_f

    :cond_14
    move v3, v12

    :goto_f
    invoke-virtual {v8, v3}, Landroid/widget/Button;->setVisibility(I)V

    if-eqz v0, :cond_15

    if-nez v4, :cond_15

    .line 363
    iget-boolean v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->waitingForStep:Z

    if-nez v3, :cond_15

    if-eqz v5, :cond_15

    goto :goto_10

    :cond_15
    move v1, v2

    .line 364
    :goto_10
    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->retryButton:Landroid/widget/Button;

    if-eqz v1, :cond_16

    move v8, v2

    goto :goto_11

    :cond_16
    move v8, v12

    :goto_11
    invoke-virtual {v3, v8}, Landroid/widget/Button;->setVisibility(I)V

    .line 365
    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->startOverButton:Landroid/widget/Button;

    if-eqz v0, :cond_17

    if-nez v4, :cond_17

    iget-boolean v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->waitingForStep:Z

    if-nez v8, :cond_17

    move v8, v2

    goto :goto_12

    :cond_17
    move v8, v12

    :goto_12
    invoke-virtual {v3, v8}, Landroid/widget/Button;->setVisibility(I)V

    .line 366
    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->playButton:Landroid/widget/Button;

    invoke-virtual {v3, v12}, Landroid/widget/Button;->setVisibility(I)V

    if-nez v0, :cond_1a

    .line 368
    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->stepView:Landroid/widget/TextView;

    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->needsSetup(Landroid/content/Context;)Z

    move-result v3

    if-eqz v3, :cond_18

    const-string v3, ""

    goto :goto_13

    :cond_18
    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->notice:Ljava/lang/String;

    if-eqz v3, :cond_19

    goto :goto_13

    :cond_19
    const-string v3, "The game is built."

    :goto_13
    invoke-virtual {v1, v3}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    goto :goto_15

    :cond_1a
    if-eqz v4, :cond_1b

    .line 369
    iget v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    const/4 v4, 0x6

    if-ne v3, v4, :cond_1b

    .line 370
    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->stepView:Landroid/widget/TextView;

    invoke-virtual {v1, v6}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    goto :goto_15

    .line 372
    :cond_1b
    iget-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->stepView:Landroid/widget/TextView;

    new-instance v4, Ljava/lang/StringBuilder;

    invoke-direct {v4}, Ljava/lang/StringBuilder;-><init>()V

    sget-object v6, Lio/github/vitarecomps/frontend/BuildActivity;->STEP_NAMES:[Ljava/lang/String;

    iget v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    aget-object v6, v6, v8

    invoke-virtual {v4, v6}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v4

    if-eqz v1, :cond_1c

    const-string v1, ": stopped"

    goto :goto_14

    :cond_1c
    const-string v1, "\u2026"

    :goto_14
    invoke-virtual {v4, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v1

    invoke-virtual {v1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v1

    invoke-virtual {v3, v1}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    :goto_15
    if-nez v0, :cond_1d

    .line 373
    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->needsSetup(Landroid/content/Context;)Z

    move-result v1

    if-nez v1, :cond_1d

    .line 374
    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->playButton:Landroid/widget/Button;

    invoke-virtual {v1, v2}, Landroid/widget/Button;->setVisibility(I)V

    .line 375
    :cond_1d
    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->errorView:Landroid/widget/TextView;

    if-eqz v0, :cond_1e

    if-eqz v5, :cond_1e

    goto :goto_16

    :cond_1e
    const-string v5, ""

    :goto_16
    invoke-virtual {v1, v5}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    .line 376
    new-instance v0, Ljava/lang/StringBuilder;

    invoke-direct {v0}, Ljava/lang/StringBuilder;-><init>()V

    .line 377
    invoke-interface {v7}, Ljava/util/List;->size()I

    move-result v1

    add-int/lit8 v1, v1, -0x3c

    invoke-static {v2, v1}, Ljava/lang/Math;->max(II)I

    move-result v1

    :goto_17
    invoke-interface {v7}, Ljava/util/List;->size()I

    move-result v2

    if-ge v1, v2, :cond_1f

    .line 378
    invoke-interface {v7, v1}, Ljava/util/List;->get(I)Ljava/lang/Object;

    move-result-object v2

    check-cast v2, Ljava/lang/String;

    invoke-virtual {v0, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2, v10}, Ljava/lang/StringBuilder;->append(C)Ljava/lang/StringBuilder;

    add-int/lit8 v1, v1, 0x1

    goto :goto_17

    .line 379
    :cond_1f
    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logView:Landroid/widget/TextView;

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    invoke-virtual {v1, v0}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    .line 380
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logScroll:Landroid/widget/ScrollView;

    new-instance v1, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda1;

    invoke-direct {v1, p0}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda1;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    invoke-virtual {v0, v1}, Landroid/widget/ScrollView;->post(Ljava/lang/Runnable;)Z

    return-void

    :catchall_0
    move-exception p0

    .line 335
    :try_start_1
    monitor-exit v3
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    throw p0
.end method

.method private run()V
    .locals 8

    .line 522
    sget-object v0, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    monitor-enter v0

    .line 523
    :try_start_0
    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    const/4 v2, 0x0

    iput-object v2, v1, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    .line 524
    monitor-exit v0
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    .line 525
    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->gameDir(Landroid/content/Context;)Ljava/io/File;

    move-result-object v0

    .line 526
    iget v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    const/4 v2, 0x3

    const/4 v3, 0x4

    const/4 v4, 0x2

    const/4 v5, 0x1

    const/4 v6, 0x0

    packed-switch v1, :pswitch_data_0

    .line 576
    iput v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    .line 577
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    goto/16 :goto_2

    .line 560
    :pswitch_0
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    if-nez v0, :cond_0

    .line 561
    iput v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    .line 562
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    goto/16 :goto_2

    .line 565
    :cond_0
    new-instance v0, Ljava/util/ArrayList;

    invoke-direct {v0}, Ljava/util/ArrayList;-><init>()V

    .line 566
    const-string v1, "--install-content"

    invoke-interface {v0, v1}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 567
    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    const-string v2, "uri"

    invoke-virtual {v1, v2}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v1

    invoke-interface {v0, v1}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 568
    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    const-string v2, "license"

    invoke-virtual {v1, v2}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v1

    invoke-virtual {v1}, Ljava/lang/String;->isEmpty()Z

    move-result v1

    if-nez v1, :cond_1

    .line 569
    const-string v1, "--license"

    invoke-interface {v0, v1}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 570
    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    const-string v2, "license"

    invoke-virtual {v1, v2}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v1

    invoke-interface {v0, v1}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 572
    :cond_1
    new-array v1, v6, [Ljava/lang/String;

    invoke-interface {v0, v1}, Ljava/util/List;->toArray([Ljava/lang/Object;)[Ljava/lang/Object;

    move-result-object v0

    check-cast v0, [Ljava/lang/String;

    invoke-direct {p0, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->frontend([Ljava/lang/String;)V

    goto/16 :goto_2

    :goto_0
    :pswitch_1
    const/4 v1, 0x6

    .line 557
    new-array v1, v1, [Ljava/lang/String;

    const-string v7, "build"

    aput-object v7, v1, v6

    invoke-virtual {v0}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v0

    aput-object v0, v1, v5

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->work()Ljava/io/File;

    move-result-object v0

    invoke-virtual {v0}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v0

    aput-object v0, v1, v4

    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildTools;->toolchain(Landroid/content/Context;)Ljava/io/File;

    move-result-object v0

    invoke-virtual {v0}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v0

    aput-object v0, v1, v2

    const-string v0, "--stamp"

    aput-object v0, v1, v3

    const/4 v0, 0x5

    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->stamp(Landroid/content/Context;)Ljava/lang/String;

    move-result-object v2

    aput-object v2, v1, v0

    invoke-direct {p0, v1}, Lio/github/vitarecomps/frontend/BuildActivity;->service([Ljava/lang/String;)V

    goto/16 :goto_2

    .line 554
    :pswitch_2
    invoke-static {p0}, Lio/github/vitarecomps/frontend/OfflineImports;->importedReady(Landroid/content/Context;)Z

    move-result v7

    if-eqz v7, :cond_2

    const/4 v7, 0x6

    iput v7, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    goto/16 :goto_0

    :cond_2
    new-array v0, v4, [Ljava/lang/String;

    const-string v1, "--dump-elfs"

    aput-object v1, v0, v6

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->dumpSeconds()I

    move-result v1

    invoke-static {v1}, Ljava/lang/String;->valueOf(I)Ljava/lang/String;

    move-result-object v1

    aput-object v1, v0, v5

    invoke-direct {p0, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->frontend([Ljava/lang/String;)V

    goto/16 :goto_2

    .line 551
    :pswitch_3
    new-array v1, v4, [Ljava/lang/String;

    const-string v2, "check"

    aput-object v2, v1, v6

    invoke-virtual {v0}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v0

    aput-object v0, v1, v5

    invoke-direct {p0, v1}, Lio/github/vitarecomps/frontend/BuildActivity;->service([Ljava/lang/String;)V

    goto/16 :goto_2

    .line 537
    :pswitch_4
    new-instance v0, Ljava/util/ArrayList;

    invoke-direct {v0}, Ljava/util/ArrayList;-><init>()V

    move v1, v6

    .line 538
    :goto_1
    iget-object v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v2}, Lorg/json/JSONArray;->length()I

    move-result v2

    if-ge v1, v2, :cond_4

    .line 539
    iget-object v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v2, v1}, Lorg/json/JSONArray;->optJSONObject(I)Lorg/json/JSONObject;

    move-result-object v2

    .line 540
    const-string v3, "--install-content"

    invoke-interface {v0, v3}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 541
    const-string v3, "uri"

    invoke-virtual {v2, v3}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v3

    invoke-interface {v0, v3}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 542
    const-string v3, "license"

    invoke-virtual {v2, v3}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/String;->isEmpty()Z

    move-result v3

    if-nez v3, :cond_3

    .line 543
    const-string v3, "--license"

    invoke-interface {v0, v3}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 544
    const-string v3, "license"

    invoke-virtual {v2, v3}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v2

    invoke-interface {v0, v2}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    :cond_3
    add-int/lit8 v1, v1, 0x1

    goto :goto_1

    .line 547
    :cond_4
    new-array v1, v6, [Ljava/lang/String;

    invoke-interface {v0, v1}, Ljava/util/List;->toArray([Ljava/lang/Object;)[Ljava/lang/Object;

    move-result-object v0

    check-cast v0, [Ljava/lang/String;

    invoke-direct {p0, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->frontend([Ljava/lang/String;)V

    goto :goto_2

    .line 531
    :pswitch_5
    new-instance v0, Ljava/io/File;

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->work()Ljava/io/File;

    move-result-object v1

    const-string v7, "firmware"

    invoke-direct {v0, v1, v7}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    .line 532
    new-array v1, v3, [Ljava/lang/String;

    const-string v3, "--install-firmware"

    aput-object v3, v1, v6

    new-instance v3, Ljava/io/File;

    const-string v6, "PSVUPDAT.PUP"

    invoke-direct {v3, v0, v6}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    invoke-virtual {v3}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v3

    aput-object v3, v1, v5

    const-string v3, "--install-firmware"

    aput-object v3, v1, v4

    new-instance v3, Ljava/io/File;

    const-string v4, "PSP2UPDAT.PUP"

    invoke-direct {v3, v0, v4}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    .line 533
    invoke-virtual {v3}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v0

    aput-object v0, v1, v2

    .line 532
    invoke-direct {p0, v1}, Lio/github/vitarecomps/frontend/BuildActivity;->frontend([Ljava/lang/String;)V

    goto :goto_2

    .line 528
    :pswitch_6
    new-array v0, v4, [Ljava/lang/String;

    const-string v1, "firmware"

    aput-object v1, v0, v6

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->work()Ljava/io/File;

    move-result-object v1

    invoke-virtual {v1}, Ljava/io/File;->getPath()Ljava/lang/String;

    move-result-object v1

    aput-object v1, v0, v5

    invoke-direct {p0, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->service([Ljava/lang/String;)V

    .line 579
    :goto_2
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->refresh()V

    return-void

    :catchall_0
    move-exception p0

    .line 524
    :try_start_1
    monitor-exit v0
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    throw p0

    :pswitch_data_0
    .packed-switch 0x1
        :pswitch_6
        :pswitch_5
        :pswitch_4
        :pswitch_3
        :pswitch_2
        :pswitch_1
        :pswitch_0
    .end packed-switch
.end method

.method private save()V
    .locals 3

    .line 384
    const-string v0, "build"

    const/4 v1, 0x0

    invoke-virtual {p0, v0, v1}, Lio/github/vitarecomps/frontend/BuildActivity;->getSharedPreferences(Ljava/lang/String;I)Landroid/content/SharedPreferences;

    move-result-object v0

    invoke-interface {v0}, Landroid/content/SharedPreferences;->edit()Landroid/content/SharedPreferences$Editor;

    move-result-object v0

    const-string v1, "step"

    iget v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    invoke-interface {v0, v1, v2}, Landroid/content/SharedPreferences$Editor;->putInt(Ljava/lang/String;I)Landroid/content/SharedPreferences$Editor;

    move-result-object v0

    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    .line 385
    invoke-virtual {v1}, Lorg/json/JSONArray;->toString()Ljava/lang/String;

    move-result-object v1

    const-string v2, "parts"

    invoke-interface {v0, v2, v1}, Landroid/content/SharedPreferences$Editor;->putString(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;

    move-result-object v0

    .line 386
    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    const-string v1, "addon"

    if-eqz p0, :cond_0

    .line 387
    invoke-virtual {p0}, Lorg/json/JSONObject;->toString()Ljava/lang/String;

    move-result-object p0

    invoke-interface {v0, v1, p0}, Landroid/content/SharedPreferences$Editor;->putString(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;

    goto :goto_0

    .line 389
    :cond_0
    invoke-interface {v0, v1}, Landroid/content/SharedPreferences$Editor;->remove(Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;

    .line 390
    :goto_0
    invoke-interface {v0}, Landroid/content/SharedPreferences$Editor;->apply()V

    return-void
.end method

.method private service([Ljava/lang/String;)V
    .locals 5

    .line 636
    sget-object v0, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    monitor-enter v0

    .line 637
    :try_start_0
    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-wide v1, v1, Lio/github/vitarecomps/frontend/BuildService$State;->generation:J

    const-wide/16 v3, 0x1

    add-long/2addr v1, v3

    iput-wide v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->serviceGeneration:J

    .line 638
    monitor-exit v0
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    .line 639
    new-instance v0, Landroid/content/Intent;

    const-class v1, Lio/github/vitarecomps/frontend/BuildService;

    invoke-direct {v0, p0, v1}, Landroid/content/Intent;-><init>(Landroid/content/Context;Ljava/lang/Class;)V

    const-string v1, "args"

    invoke-virtual {v0, v1, p1}, Landroid/content/Intent;->putExtra(Ljava/lang/String;[Ljava/lang/String;)Landroid/content/Intent;

    move-result-object p1

    const-string v0, "title"

    sget-object v1, Lio/github/vitarecomps/frontend/BuildActivity;->STEP_NAMES:[Ljava/lang/String;

    iget v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    aget-object v1, v1, v2

    .line 640
    invoke-virtual {p1, v0, v1}, Landroid/content/Intent;->putExtra(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;

    move-result-object p1

    .line 641
    invoke-virtual {p0, p1}, Lio/github/vitarecomps/frontend/BuildActivity;->startForegroundService(Landroid/content/Intent;)Landroid/content/ComponentName;

    return-void

    :catchall_0
    move-exception p0

    .line 638
    :try_start_1
    monitor-exit v0
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    throw p0
.end method

.method static stamp(Landroid/content/Context;)Ljava/lang/String;
    .locals 8

    .line 113
    const-string v0, "BuildActivity"

    const-string v1, "buildtools.json"

    const-string v2, ""

    .line 114
    :try_start_0
    invoke-virtual {p0}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v3

    invoke-virtual {v3, v1}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;

    move-result-object v3
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    .line 115
    :try_start_1
    new-instance v4, Lorg/json/JSONObject;

    new-instance v5, Ljava/lang/String;

    invoke-static {v3}, Lio/github/vitarecomps/frontend/BuildActivity;->readAll(Ljava/io/InputStream;)[B

    move-result-object v6

    sget-object v7, Ljava/nio/charset/StandardCharsets;->UTF_8:Ljava/nio/charset/Charset;

    invoke-direct {v5, v6, v7}, Ljava/lang/String;-><init>([BLjava/nio/charset/Charset;)V

    invoke-direct {v4, v5}, Lorg/json/JSONObject;-><init>(Ljava/lang/String;)V

    const-string v5, "zip_sha256"

    invoke-virtual {v4, v5}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object v2
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    if-eqz v3, :cond_1

    .line 116
    :try_start_2
    invoke-virtual {v3}, Ljava/io/InputStream;->close()V
    :try_end_2
    .catch Ljava/lang/Exception; {:try_start_2 .. :try_end_2} :catch_0

    goto :goto_1

    :catchall_0
    move-exception v4

    if-eqz v3, :cond_0

    .line 114
    :try_start_3
    invoke-virtual {v3}, Ljava/io/InputStream;->close()V
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_1

    goto :goto_0

    :catchall_1
    move-exception v3

    :try_start_4
    invoke-virtual {v4, v3}, Ljava/lang/Throwable;->addSuppressed(Ljava/lang/Throwable;)V

    :cond_0
    :goto_0
    throw v4
    :try_end_4
    .catch Ljava/lang/Exception; {:try_start_4 .. :try_end_4} :catch_0

    :catch_0
    move-exception v3

    .line 117
    invoke-static {v0, v1, v3}, Landroid/util/Log;->w(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Throwable;)I

    .line 121
    :cond_1
    :goto_1
    :try_start_5
    invoke-virtual {p0}, Landroid/content/Context;->getPackageManager()Landroid/content/pm/PackageManager;

    move-result-object v1

    invoke-virtual {p0}, Landroid/content/Context;->getPackageName()Ljava/lang/String;

    move-result-object p0

    const/4 v3, 0x0

    invoke-virtual {v1, p0, v3}, Landroid/content/pm/PackageManager;->getPackageInfo(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;

    move-result-object p0

    invoke-virtual {p0}, Landroid/content/pm/PackageInfo;->getLongVersionCode()J

    move-result-wide v0
    :try_end_5
    .catch Landroid/content/pm/PackageManager$NameNotFoundException; {:try_start_5 .. :try_end_5} :catch_1

    goto :goto_2

    :catch_1
    move-exception p0

    .line 123
    const-string v1, "version"

    invoke-static {v0, v1, p0}, Landroid/util/Log;->w(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Throwable;)I

    const-wide/16 v0, 0x0

    .line 125
    :goto_2
    new-instance p0, Ljava/lang/StringBuilder;

    invoke-direct {p0}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual {p0, v0, v1}, Ljava/lang/StringBuilder;->append(J)Ljava/lang/StringBuilder;

    move-result-object p0

    const-string v0, "-"

    invoke-virtual {p0, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p0

    invoke-virtual {p0, v2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p0

    invoke-virtual {p0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p0

    return-object p0
.end method

.method private startAddon()V
    .locals 3

    .line 395
    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildService;->logFile(Landroid/content/Context;)Ljava/io/File;

    move-result-object v0

    invoke-virtual {v0}, Ljava/io/File;->delete()Z

    .line 396
    sget-object v0, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    monitor-enter v0

    .line 397
    :try_start_0
    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-object v1, v1, Lio/github/vitarecomps/frontend/BuildService$State;->lines:Ljava/util/List;

    invoke-interface {v1}, Ljava/util/List;->clear()V

    .line 398
    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    const/4 v2, 0x0

    iput-object v2, v1, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    .line 399
    monitor-exit v0
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    .line 400
    iput-object v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->notice:Ljava/lang/String;

    const/4 v0, 0x7

    .line 401
    iput v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    .line 402
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    .line 403
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->run()V

    return-void

    :catchall_0
    move-exception p0

    .line 399
    :try_start_1
    monitor-exit v0
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    throw p0
.end method

.method private startSetup()V
    .locals 4

    .line 503
    sget v0, Landroid/os/Build$VERSION;->SDK_INT:I

    const/16 v1, 0x21

    const/4 v2, 0x1

    if-lt v0, v1, :cond_0

    const-string v0, "android.permission.POST_NOTIFICATIONS"

    invoke-virtual {p0, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->checkSelfPermission(Ljava/lang/String;)I

    move-result v0

    if-eqz v0, :cond_0

    .line 504
    new-array v0, v2, [Ljava/lang/String;

    const-string v1, "android.permission.POST_NOTIFICATIONS"

    const/4 v3, 0x0

    aput-object v1, v0, v3

    invoke-virtual {p0, v0, v3}, Lio/github/vitarecomps/frontend/BuildActivity;->requestPermissions([Ljava/lang/String;I)V

    .line 505
    :cond_0
    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildService;->logFile(Landroid/content/Context;)Ljava/io/File;

    move-result-object v0

    invoke-virtual {v0}, Ljava/io/File;->delete()Z

    .line 506
    sget-object v0, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    monitor-enter v0

    .line 507
    :try_start_0
    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-object v1, v1, Lio/github/vitarecomps/frontend/BuildService$State;->lines:Ljava/util/List;

    invoke-interface {v1}, Ljava/util/List;->clear()V

    .line 508
    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    const/4 v3, 0x0

    iput-object v3, v1, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    .line 509
    monitor-exit v0
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    .line 511
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {v0}, Lorg/json/JSONArray;->length()I

    move-result v0

    if-nez v0, :cond_1

    iget-boolean v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->rebuild:Z

    if-eqz v0, :cond_1

    const/4 v2, 0x6

    :cond_1
    iput v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    .line 512
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    .line 513
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->run()V

    return-void

    :catchall_0
    move-exception p0

    .line 509
    :try_start_1
    monitor-exit v0
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    throw p0
.end method

.method private text(Ljava/lang/String;IZ)Landroid/widget/TextView;
    .locals 1

    .line 306
    new-instance v0, Landroid/widget/TextView;

    invoke-direct {v0, p0}, Landroid/widget/TextView;-><init>(Landroid/content/Context;)V

    .line 307
    invoke-virtual {v0, p1}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    const/4 p0, 0x2

    int-to-float p1, p2

    .line 308
    invoke-virtual {v0, p0, p1}, Landroid/widget/TextView;->setTextSize(IF)V

    if-eqz p3, :cond_0

    .line 310
    sget-object p0, Landroid/graphics/Typeface;->DEFAULT_BOLD:Landroid/graphics/Typeface;

    invoke-virtual {v0, p0}, Landroid/widget/TextView;->setTypeface(Landroid/graphics/Typeface;)V

    :cond_0
    return-object v0
.end method

.method private work()Ljava/io/File;
    .locals 2

    .line 517
    new-instance v0, Ljava/io/File;

    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->getFilesDir()Ljava/io/File;

    move-result-object p0

    const-string v1, "setup-work"

    invoke-direct {v0, p0, v1}, Ljava/io/File;-><init>(Ljava/io/File;Ljava/lang/String;)V

    return-object v0
.end method


# virtual methods
.method protected onActivityResult(IILandroid/content/Intent;)V
    .locals 6

    invoke-static {p0, p1, p2, p3}, Lio/github/vitarecomps/frontend/OfflineImports;->result(Landroid/app/Activity;IILandroid/content/Intent;)Z

    move-result v0

    if-eqz v0, :cond_0

    return-void

    :cond_0
    const-string v0, "Now pick the licence of "

    .line 427
    invoke-super {p0, p1, p2, p3}, Landroid/app/Activity;->onActivityResult(IILandroid/content/Intent;)V

    invoke-static {p0, p1, p2, p3}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->afterStep(Landroid/app/Activity;IILandroid/content/Intent;)V
    invoke-static {p0}, Lio/github/vitarecomps/frontend/MainInstallProgress;->stop(Landroid/app/Activity;)V

    iget v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    invoke-static {p0, v0, p2, p3}, Lio/github/vitarecomps/frontend/OfflineImports;->nativeStepResult(Landroid/app/Activity;IILandroid/content/Intent;)V

    const-string v0, "Now pick the licence of "

    const/4 v1, 0x3

    const/4 v2, 0x0

    const/4 v3, 0x1

    const/4 v4, -0x1

    if-ne p1, v1, :cond_5

    .line 429
    iput-boolean v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->waitingForStep:Z

    if-eqz p3, :cond_1

    .line 430
    const-string p1, "exit_code"

    invoke-virtual {p3, p1, v4}, Landroid/content/Intent;->getIntExtra(Ljava/lang/String;I)I

    move-result p1

    goto :goto_0

    :cond_1
    move p1, v4

    .line 431
    :goto_0
    new-instance p3, Ljava/lang/StringBuilder;

    invoke-direct {p3}, Ljava/lang/StringBuilder;-><init>()V

    sget-object v0, Lio/github/vitarecomps/frontend/BuildActivity;->STEP_NAMES:[Ljava/lang/String;

    iget v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    aget-object v1, v0, v1

    invoke-virtual {p3, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p3

    const-string v1, ": exit "

    invoke-virtual {p3, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p3

    invoke-virtual {p3, p1}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object p3

    invoke-virtual {p3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p3

    invoke-static {p0, p3}, Lio/github/vitarecomps/frontend/BuildService;->append(Landroid/content/Context;Ljava/lang/String;)V

    .line 432
    iget p3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    const/4 v1, 0x7

    if-ne p3, v1, :cond_3

    if-ne p2, v4, :cond_3

    if-nez p1, :cond_3

    .line 434
    new-instance p1, Ljava/lang/StringBuilder;

    const-string p2, "Installed: "

    invoke-direct {p1, p2}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    iget-object p2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    if-eqz p2, :cond_2

    const-string p3, "name"

    invoke-virtual {p2, p3}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object p2

    goto :goto_1

    :cond_2
    const-string p2, "the add-on"

    :goto_1
    invoke-virtual {p1, p2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    const-string p2, ". The game is built."

    invoke-virtual {p1, p2}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p1

    iput-object p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->notice:Ljava/lang/String;

    const/4 p1, 0x0

    .line 436
    iput-object p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    .line 437
    iput v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    .line 438
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    .line 439
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->refresh()V

    .line 440
    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->playButton:Landroid/widget/Button;

    invoke-virtual {p0}, Landroid/widget/Button;->requestFocus()Z

    return-void

    :cond_3
    if-ne p2, v4, :cond_4

    if-nez p1, :cond_4

    add-int/2addr p3, v3

    .line 444
    iput p3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    .line 445
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    .line 446
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->run()V

    return-void

    .line 448
    :cond_4
    sget-object v1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    monitor-enter v1

    .line 449
    :try_start_0
    sget-object p1, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    new-instance p2, Ljava/lang/StringBuilder;

    invoke-direct {p2}, Ljava/lang/StringBuilder;-><init>()V

    iget p3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    aget-object p3, v0, p3

    invoke-virtual {p2, p3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p2

    const-string p3, " failed (the game\'s log: "

    invoke-virtual {p2, p3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p2

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->game()Lorg/json/JSONObject;

    move-result-object p3

    const-string v0, "id"

    invoke-virtual {p3, v0}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object p3

    invoke-virtual {p2, p3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p2

    const-string p3, "/vita3k.log in the app\'s folder)."

    invoke-virtual {p2, p3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p2

    invoke-virtual {p2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p2

    iput-object p2, p1, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    .line 451
    monitor-exit v1
    :try_end_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    .line 452
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->refresh()V

    return-void

    :catchall_0
    move-exception p0

    .line 451
    :try_start_1
    monitor-exit v1
    :try_end_1
    .catchall {:try_start_1 .. :try_end_1} :catchall_0

    throw p0

    :cond_5
    if-ne p2, v4, :cond_6

    if-eqz p3, :cond_6

    .line 456
    invoke-virtual {p3}, Landroid/content/Intent;->getData()Landroid/net/Uri;

    move-result-object p2

    if-eqz p2, :cond_6

    move v2, v3

    :cond_6
    const/4 p2, 0x5

    if-ne p1, p2, :cond_7

    if-nez v2, :cond_7

    .line 459
    iget-object v1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    if-eqz v1, :cond_7

    .line 460
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->startAddon()V

    return-void

    :cond_7
    if-nez v2, :cond_8

    goto/16 :goto_3

    .line 465
    :cond_8
    invoke-virtual {p3}, Landroid/content/Intent;->getData()Landroid/net/Uri;

    move-result-object p3

    .line 467
    :try_start_2
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->getContentResolver()Landroid/content/ContentResolver;

    move-result-object v1

    invoke-virtual {v1, p3, v3}, Landroid/content/ContentResolver;->takePersistableUriPermission(Landroid/net/Uri;I)V
    :try_end_2
    .catch Ljava/lang/SecurityException; {:try_start_2 .. :try_end_2} :catch_0

    goto :goto_2

    :catch_0
    move-exception v1

    .line 469
    const-string v2, "BuildActivity"

    new-instance v4, Ljava/lang/StringBuilder;

    const-string v5, "no persistable permission for "

    invoke-direct {v4, v5}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-virtual {v4, p3}, Ljava/lang/StringBuilder;->append(Ljava/lang/Object;)Ljava/lang/StringBuilder;

    move-result-object v4

    invoke-virtual {v4}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v4

    invoke-static {v2, v4, v1}, Landroid/util/Log;->w(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Throwable;)I

    :goto_2
    const/4 v1, 0x4

    if-ne p1, v1, :cond_a

    .line 473
    :try_start_3
    new-instance p1, Lorg/json/JSONObject;

    invoke-direct {p1}, Lorg/json/JSONObject;-><init>()V

    const-string v1, "uri"

    invoke-virtual {p3}, Landroid/net/Uri;->toString()Ljava/lang/String;

    move-result-object v2

    invoke-virtual {p1, v1, v2}, Lorg/json/JSONObject;->put(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;

    move-result-object p1

    const-string v1, "name"

    invoke-direct {p0, p3}, Lio/github/vitarecomps/frontend/BuildActivity;->displayName(Landroid/net/Uri;)Ljava/lang/String;

    move-result-object p3

    invoke-virtual {p1, v1, p3}, Lorg/json/JSONObject;->put(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;

    move-result-object p1

    iput-object p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    .line 474
    const-string p3, "name"

    invoke-virtual {p1, p3}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object p1

    sget-object p3, Ljava/util/Locale;->ROOT:Ljava/util/Locale;

    invoke-virtual {p1, p3}, Ljava/lang/String;->toLowerCase(Ljava/util/Locale;)Ljava/lang/String;

    move-result-object p1

    const-string p3, ".pkg"

    invoke-virtual {p1, p3}, Ljava/lang/String;->endsWith(Ljava/lang/String;)Z

    move-result p1

    if-eqz p1, :cond_9

    .line 476
    new-instance p1, Ljava/lang/StringBuilder;

    invoke-direct {p1, v0}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    iget-object p3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    const-string v0, "name"

    invoke-virtual {p3, v0}, Lorg/json/JSONObject;->optString(Ljava/lang/String;)Ljava/lang/String;

    move-result-object p3

    invoke-virtual {p1, p3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    const-string p3, " (work.bin or .rif)."

    invoke-virtual {p1, p3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p1

    iput-object p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->notice:Ljava/lang/String;

    .line 477
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->refresh()V

    .line 478
    invoke-direct {p0, p2}, Lio/github/vitarecomps/frontend/BuildActivity;->pick(I)V

    goto :goto_3

    .line 480
    :cond_9
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->startAddon()V

    goto :goto_3

    :catch_1
    move-exception p1

    goto :goto_4

    :cond_a
    if-ne p1, p2, :cond_b

    .line 482
    iget-object p2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    if-eqz p2, :cond_b

    .line 483
    const-string p1, "license"

    invoke-virtual {p3}, Landroid/net/Uri;->toString()Ljava/lang/String;

    move-result-object v0

    invoke-virtual {p2, p1, v0}, Lorg/json/JSONObject;->put(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;

    move-result-object p1

    const-string p2, "license_name"

    invoke-direct {p0, p3}, Lio/github/vitarecomps/frontend/BuildActivity;->displayName(Landroid/net/Uri;)Ljava/lang/String;

    move-result-object p3

    invoke-virtual {p1, p2, p3}, Lorg/json/JSONObject;->put(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;

    .line 484
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->startAddon()V

    :goto_3
    return-void

    :cond_b
    if-ne p1, v3, :cond_c

    .line 487
    iget-object p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    new-instance p2, Lorg/json/JSONObject;

    invoke-direct {p2}, Lorg/json/JSONObject;-><init>()V

    const-string v0, "uri"

    invoke-virtual {p3}, Landroid/net/Uri;->toString()Ljava/lang/String;

    move-result-object v1

    invoke-virtual {p2, v0, v1}, Lorg/json/JSONObject;->put(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;

    move-result-object p2

    const-string v0, "name"

    invoke-direct {p0, p3}, Lio/github/vitarecomps/frontend/BuildActivity;->displayName(Landroid/net/Uri;)Ljava/lang/String;

    move-result-object p3

    invoke-virtual {p2, v0, p3}, Lorg/json/JSONObject;->put(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;

    move-result-object p2

    invoke-virtual {p1, p2}, Lorg/json/JSONArray;->put(Ljava/lang/Object;)Lorg/json/JSONArray;

    goto :goto_5

    :cond_c
    const/4 p2, 0x2

    if-ne p1, p2, :cond_d

    .line 488
    iget-object p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {p1}, Lorg/json/JSONArray;->length()I

    move-result p1

    if-lez p1, :cond_d

    .line 489
    iget-object p1, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    invoke-virtual {p1}, Lorg/json/JSONArray;->length()I

    move-result p2

    sub-int/2addr p2, v3

    invoke-virtual {p1, p2}, Lorg/json/JSONArray;->getJSONObject(I)Lorg/json/JSONObject;

    move-result-object p1

    .line 490
    const-string p2, "license"

    invoke-virtual {p3}, Landroid/net/Uri;->toString()Ljava/lang/String;

    move-result-object v0

    invoke-virtual {p1, p2, v0}, Lorg/json/JSONObject;->put(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;

    move-result-object p1

    const-string p2, "license_name"

    invoke-direct {p0, p3}, Lio/github/vitarecomps/frontend/BuildActivity;->displayName(Landroid/net/Uri;)Ljava/lang/String;

    move-result-object p3

    invoke-virtual {p1, p2, p3}, Lorg/json/JSONObject;->put(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;
    :try_end_3
    .catch Ljava/lang/Exception; {:try_start_3 .. :try_end_3} :catch_1

    goto :goto_5

    .line 493
    :goto_4
    const-string p2, "BuildActivity"

    const-string p3, "parts"

    invoke-static {p2, p3, p1}, Landroid/util/Log;->e(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Throwable;)I

    .line 495
    :cond_d
    :goto_5
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->save()V

    .line 496
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->refresh()V

    .line 497
    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->buildButton:Landroid/widget/Button;

    invoke-virtual {p0}, Landroid/widget/Button;->requestFocus()Z

    return-void
.end method

.method protected onCreate(Landroid/os/Bundle;)V
    .locals 9

    .line 161
    invoke-super {p0, p1}, Landroid/app/Activity;->onCreate(Landroid/os/Bundle;)V

    .line 162
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->getWindow()Landroid/view/Window;

    move-result-object v0

    const/16 v1, 0x80

    invoke-virtual {v0, v1}, Landroid/view/Window;->addFlags(I)V

    .line 163
    const-string v0, "build"

    const/4 v1, 0x0

    invoke-virtual {p0, v0, v1}, Lio/github/vitarecomps/frontend/BuildActivity;->getSharedPreferences(Ljava/lang/String;I)Landroid/content/SharedPreferences;

    move-result-object v0

    .line 164
    const-string v2, "step"

    invoke-interface {v0, v2, v1}, Landroid/content/SharedPreferences;->getInt(Ljava/lang/String;I)I

    move-result v2

    iput v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    .line 166
    :try_start_0
    new-instance v2, Lorg/json/JSONArray;

    const-string v3, "parts"

    const-string v4, "[]"

    invoke-interface {v0, v3, v4}, Landroid/content/SharedPreferences;->getString(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v3

    invoke-direct {v2, v3}, Lorg/json/JSONArray;-><init>(Ljava/lang/String;)V

    iput-object v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0

    goto :goto_0

    .line 168
    :catch_0
    new-instance v2, Lorg/json/JSONArray;

    invoke-direct {v2}, Lorg/json/JSONArray;-><init>()V

    iput-object v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->parts:Lorg/json/JSONArray;

    :goto_0
    const/4 v2, 0x0

    .line 171
    :try_start_1
    const-string v3, "addon"

    invoke-interface {v0, v3}, Landroid/content/SharedPreferences;->contains(Ljava/lang/String;)Z

    move-result v3

    if-eqz v3, :cond_0

    new-instance v3, Lorg/json/JSONObject;

    const-string v4, "addon"

    const-string v5, "{}"

    invoke-interface {v0, v4, v5}, Landroid/content/SharedPreferences;->getString(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;

    move-result-object v0

    invoke-direct {v3, v0}, Lorg/json/JSONObject;-><init>(Ljava/lang/String;)V

    goto :goto_1

    :cond_0
    move-object v3, v2

    :goto_1
    iput-object v3, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;
    :try_end_1
    .catch Ljava/lang/Exception; {:try_start_1 .. :try_end_1} :catch_1

    goto :goto_2

    .line 173
    :catch_1
    iput-object v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addon:Lorg/json/JSONObject;

    :goto_2
    const/16 v0, 0x18

    .line 176
    invoke-direct {p0, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->dp(I)I

    move-result v0

    .line 177
    new-instance v2, Landroid/widget/LinearLayout;

    invoke-direct {v2, p0}, Landroid/widget/LinearLayout;-><init>(Landroid/content/Context;)V

    const/4 v3, 0x1

    .line 178
    invoke-virtual {v2, v3}, Landroid/widget/LinearLayout;->setOrientation(I)V

    .line 179
    invoke-virtual {v2, v0, v0, v0, v0}, Landroid/widget/LinearLayout;->setPadding(IIII)V

    .line 181
    new-instance v0, Ljava/lang/StringBuilder;

    const-string v4, "Set up "

    invoke-direct {v0, v4}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->gameName()Ljava/lang/String;

    move-result-object v4

    invoke-virtual {v0, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    const/16 v4, 0x1a

    invoke-direct {p0, v0, v4, v3}, Lio/github/vitarecomps/frontend/BuildActivity;->text(Ljava/lang/String;IZ)Landroid/widget/TextView;

    move-result-object v0

    .line 182
    invoke-virtual {v2, v0}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 183
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->canRebuild()Z

    move-result v0

    iput-boolean v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->rebuild:Z

    if-eqz v0, :cond_1

    .line 186
    new-instance v0, Ljava/lang/StringBuilder;

    const-string v4, "This version of the app builds the game\'s native code again, from the game installed here: about "

    invoke-direct {v0, v4}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->buildMinutes()I

    move-result v4

    invoke-virtual {v0, v4}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v0

    const-string v4, " minutes on a fast device, with 5 GB of free space while it runs. Keep the device charging. Your dump isn\'t needed (add it only to install the game again)."

    invoke-virtual {v0, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    goto :goto_3

    .line 194
    :cond_1
    new-instance v0, Ljava/lang/StringBuilder;

    const-string v4, "The game is built on this device from your own copy of it: a dump of the PS Vita game (a NoNpDrm .zip or .vpk; zip a dumped folder first), or a .pkg with its licence file (work.bin or .rif). Add the game\'s last update after it if the dump lacks it (its .pkg), and add-ons (DLC) the same way, or a rePatch mod (a .zip with its rePatch folder); add-ons and mods can also be added later, from the app icon\'s long-press menu. The PS Vita firmware comes from Sony\'s servers (190 MB).\n\nBuilding takes about "

    invoke-direct {v0, v4}, Ljava/lang/StringBuilder;-><init>(Ljava/lang/String;)V

    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->buildMinutes()I

    move-result v4

    invoke-virtual {v0, v4}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v0

    const-string v4, " minutes on a fast device and needs about 10 GB of free space (5 GB of it only while building). Keep the device charging. Nothing leaves the device."

    invoke-virtual {v0, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    :goto_3
    const/16 v4, 0xf

    .line 184
    invoke-direct {p0, v0, v4, v1}, Lio/github/vitarecomps/frontend/BuildActivity;->text(Ljava/lang/String;IZ)Landroid/widget/TextView;

    move-result-object v0

    const/16 v5, 0x8

    .line 196
    invoke-direct {p0, v5}, Lio/github/vitarecomps/frontend/BuildActivity;->dp(I)I

    move-result v6

    const/16 v7, 0xc

    invoke-direct {p0, v7}, Lio/github/vitarecomps/frontend/BuildActivity;->dp(I)I

    move-result v8

    invoke-virtual {v0, v1, v6, v1, v8}, Landroid/widget/TextView;->setPadding(IIII)V

    .line 197
    invoke-virtual {v2, v0}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 198
    iget-object v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->formViews:Ljava/util/List;

    invoke-interface {v6, v0}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 200
    new-instance v0, Landroid/widget/LinearLayout;

    invoke-direct {v0, p0}, Landroid/widget/LinearLayout;-><init>(Landroid/content/Context;)V

    .line 201
    invoke-virtual {v0, v1}, Landroid/widget/LinearLayout;->setOrientation(I)V

    .line 202
    const-string v6, "Add the game\'s files\u2026"

    new-instance v8, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda0;

    invoke-direct {v8, p0}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda0;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    invoke-direct {p0, v6, v8}, Lio/github/vitarecomps/frontend/BuildActivity;->button(Ljava/lang/String;Landroid/view/View$OnClickListener;)Landroid/widget/Button;

    move-result-object v6

    iput-object v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addButton:Landroid/widget/Button;

    .line 203
    const-string v6, "Add a licence\u2026"

    new-instance v8, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda3;

    invoke-direct {v8, p0}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda3;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    invoke-direct {p0, v6, v8}, Lio/github/vitarecomps/frontend/BuildActivity;->button(Ljava/lang/String;Landroid/view/View$OnClickListener;)Landroid/widget/Button;

    move-result-object v6

    iput-object v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->licenseButton:Landroid/widget/Button;

    .line 204
    const-string v6, "Clear"

    new-instance v8, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda4;

    invoke-direct {v8, p0}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda4;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    invoke-direct {p0, v6, v8}, Lio/github/vitarecomps/frontend/BuildActivity;->button(Ljava/lang/String;Landroid/view/View$OnClickListener;)Landroid/widget/Button;

    move-result-object v6

    iput-object v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->clearButton:Landroid/widget/Button;

    .line 209
    iget-object v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addButton:Landroid/widget/Button;

    invoke-virtual {v0, v6}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 210
    iget-object v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->licenseButton:Landroid/widget/Button;

    invoke-virtual {v0, v6}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 211
    iget-object v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->clearButton:Landroid/widget/Button;

    invoke-virtual {v0, v6}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 212
    invoke-virtual {v2, v0}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 213
    iget-object v6, p0, Lio/github/vitarecomps/frontend/BuildActivity;->formViews:Ljava/util/List;

    invoke-interface {v6, v0}, Ljava/util/List;->add(Ljava/lang/Object;)Z

    .line 215
    const-string v0, ""

    invoke-direct {p0, v0, v4, v1}, Lio/github/vitarecomps/frontend/BuildActivity;->text(Ljava/lang/String;IZ)Landroid/widget/TextView;

    move-result-object v0

    iput-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->partsView:Landroid/widget/TextView;

    .line 216
    invoke-direct {p0, v5}, Lio/github/vitarecomps/frontend/BuildActivity;->dp(I)I

    move-result v6

    invoke-direct {p0, v5}, Lio/github/vitarecomps/frontend/BuildActivity;->dp(I)I

    move-result v5

    invoke-virtual {v0, v1, v6, v1, v5}, Landroid/widget/TextView;->setPadding(IIII)V

    .line 217
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->partsView:Landroid/widget/TextView;

    invoke-virtual {v2, v0}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 219
    new-instance v0, Landroid/widget/LinearLayout;

    invoke-direct {v0, p0}, Landroid/widget/LinearLayout;-><init>(Landroid/content/Context;)V

    .line 220
    invoke-virtual {v0, v1}, Landroid/widget/LinearLayout;->setOrientation(I)V

    .line 221
    const-string v5, "Build"

    new-instance v6, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda5;

    invoke-direct {v6, p0}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda5;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    invoke-direct {p0, v5, v6}, Lio/github/vitarecomps/frontend/BuildActivity;->button(Ljava/lang/String;Landroid/view/View$OnClickListener;)Landroid/widget/Button;

    move-result-object v5

    iput-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->buildButton:Landroid/widget/Button;

    .line 222
    const-string v5, "Try again"

    new-instance v6, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda6;

    invoke-direct {v6, p0}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda6;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    invoke-direct {p0, v5, v6}, Lio/github/vitarecomps/frontend/BuildActivity;->button(Ljava/lang/String;Landroid/view/View$OnClickListener;)Landroid/widget/Button;

    move-result-object v5

    iput-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->retryButton:Landroid/widget/Button;

    .line 226
    const-string v5, "Play"

    new-instance v6, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda7;

    invoke-direct {v6, p0}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda7;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    invoke-direct {p0, v5, v6}, Lio/github/vitarecomps/frontend/BuildActivity;->button(Ljava/lang/String;Landroid/view/View$OnClickListener;)Landroid/widget/Button;

    move-result-object v5

    iput-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->playButton:Landroid/widget/Button;

    .line 227
    const-string v5, "Start over"

    new-instance v6, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda8;

    invoke-direct {v6, p0}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda8;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    invoke-direct {p0, v5, v6}, Lio/github/vitarecomps/frontend/BuildActivity;->button(Ljava/lang/String;Landroid/view/View$OnClickListener;)Landroid/widget/Button;

    move-result-object v5

    iput-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->startOverButton:Landroid/widget/Button;

    .line 235
    const-string v5, "Add an add-on or mod\u2026"

    new-instance v6, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda9;

    invoke-direct {v6, p0}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda9;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;)V

    invoke-direct {p0, v5, v6}, Lio/github/vitarecomps/frontend/BuildActivity;->button(Ljava/lang/String;Landroid/view/View$OnClickListener;)Landroid/widget/Button;

    move-result-object v5

    iput-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addonButton:Landroid/widget/Button;

    .line 236
    iget-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->buildButton:Landroid/widget/Button;

    invoke-virtual {v0, v5}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 237
    iget-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->retryButton:Landroid/widget/Button;

    invoke-virtual {v0, v5}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 238
    iget-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->playButton:Landroid/widget/Button;

    invoke-virtual {v0, v5}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 239
    iget-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->addonButton:Landroid/widget/Button;

    invoke-virtual {v0, v5}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 240
    iget-object v5, p0, Lio/github/vitarecomps/frontend/BuildActivity;->startOverButton:Landroid/widget/Button;

    invoke-virtual {v0, v5}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 241
    invoke-virtual {v2, v0}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 243
    const-string v0, ""

    const/16 v5, 0x12

    invoke-direct {p0, v0, v5, v3}, Lio/github/vitarecomps/frontend/BuildActivity;->text(Ljava/lang/String;IZ)Landroid/widget/TextView;

    move-result-object v0

    iput-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->stepView:Landroid/widget/TextView;

    .line 244
    invoke-direct {p0, v7}, Lio/github/vitarecomps/frontend/BuildActivity;->dp(I)I

    move-result v5

    const/4 v6, 0x4

    invoke-direct {p0, v6}, Lio/github/vitarecomps/frontend/BuildActivity;->dp(I)I

    move-result v7

    invoke-virtual {v0, v1, v5, v1, v7}, Landroid/widget/TextView;->setPadding(IIII)V

    .line 245
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->stepView:Landroid/widget/TextView;

    invoke-virtual {v2, v0}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 246
    const-string v0, ""

    invoke-direct {p0, v0, v4, v1}, Lio/github/vitarecomps/frontend/BuildActivity;->text(Ljava/lang/String;IZ)Landroid/widget/TextView;

    move-result-object v0

    iput-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->errorView:Landroid/widget/TextView;

    const v4, -0x9495

    .line 247
    invoke-virtual {v0, v4}, Landroid/widget/TextView;->setTextColor(I)V

    .line 248
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->errorView:Landroid/widget/TextView;

    invoke-virtual {v2, v0}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 250
    new-instance v0, Landroid/widget/TextView;

    invoke-direct {v0, p0}, Landroid/widget/TextView;-><init>(Landroid/content/Context;)V

    iput-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logView:Landroid/widget/TextView;

    .line 251
    sget-object v4, Landroid/graphics/Typeface;->MONOSPACE:Landroid/graphics/Typeface;

    invoke-virtual {v0, v4}, Landroid/widget/TextView;->setTypeface(Landroid/graphics/Typeface;)V

    .line 252
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logView:Landroid/widget/TextView;

    const/high16 v4, 0x41300000    # 11.0f

    const/4 v5, 0x2

    invoke-virtual {v0, v5, v4}, Landroid/widget/TextView;->setTextSize(IF)V

    .line 253
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logView:Landroid/widget/TextView;

    const v4, -0x4f4f50

    invoke-virtual {v0, v4}, Landroid/widget/TextView;->setTextColor(I)V

    .line 254
    new-instance v0, Landroid/widget/ScrollView;

    invoke-direct {v0, p0}, Landroid/widget/ScrollView;-><init>(Landroid/content/Context;)V

    iput-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logScroll:Landroid/widget/ScrollView;

    .line 255
    invoke-virtual {v0, v1}, Landroid/widget/ScrollView;->setFocusable(Z)V

    .line 256
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logScroll:Landroid/widget/ScrollView;

    iget-object v4, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logView:Landroid/widget/TextView;

    invoke-virtual {v0, v4}, Landroid/widget/ScrollView;->addView(Landroid/view/View;)V

    .line 257
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->logScroll:Landroid/widget/ScrollView;

    new-instance v4, Landroid/widget/LinearLayout$LayoutParams;

    const/16 v7, 0xdc

    invoke-direct {p0, v7}, Lio/github/vitarecomps/frontend/BuildActivity;->dp(I)I

    move-result v7

    const/4 v8, -0x1

    invoke-direct {v4, v8, v7}, Landroid/widget/LinearLayout$LayoutParams;-><init>(II)V

    invoke-virtual {v2, v0, v4}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V

    .line 259
    new-instance v0, Landroid/widget/ScrollView;

    invoke-direct {v0, p0}, Landroid/widget/ScrollView;-><init>(Landroid/content/Context;)V

    .line 260
    invoke-virtual {v0, v1}, Landroid/widget/ScrollView;->setFocusable(Z)V

    .line 261
    invoke-virtual {v0, v2}, Landroid/widget/ScrollView;->addView(Landroid/view/View;)V

    .line 262
    invoke-virtual {p0, v0}, Lio/github/vitarecomps/frontend/BuildActivity;->setContentView(Landroid/view/View;)V

    invoke-static {p0}, Lio/github/vitarecomps/frontend/ImportDiagnostics;->attach(Landroid/app/Activity;)V

    invoke-static {p0}, Lio/github/vitarecomps/frontend/OfflineImports;->attach(Landroid/app/Activity;)V

    .line 264
    iget v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    if-eq v2, v5, :cond_2

    const/4 v4, 0x3

    if-eq v2, v4, :cond_2

    const/4 v4, 0x5

    if-eq v2, v4, :cond_2

    const/4 v4, 0x7

    if-ne v2, v4, :cond_4

    .line 265
    :cond_2
    sget-object v2, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    monitor-enter v2

    .line 266
    :try_start_2
    sget-object v4, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    iget-object v4, v4, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    if-nez v4, :cond_3

    .line 267
    sget-object v4, Lio/github/vitarecomps/frontend/BuildService;->state:Lio/github/vitarecomps/frontend/BuildService$State;

    new-instance v5, Ljava/lang/StringBuilder;

    invoke-direct {v5}, Ljava/lang/StringBuilder;-><init>()V

    sget-object v7, Lio/github/vitarecomps/frontend/BuildActivity;->STEP_NAMES:[Ljava/lang/String;

    iget v8, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    aget-object v7, v7, v8

    invoke-virtual {v5, v7}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v5

    const-string v7, " was interrupted: Try again goes on from there."

    invoke-virtual {v5, v7}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v5

    invoke-virtual {v5}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v5

    iput-object v5, v4, Lio/github/vitarecomps/frontend/BuildService$State;->error:Ljava/lang/String;

    .line 268
    :cond_3
    monitor-exit v2
    :try_end_2
    .catchall {:try_start_2 .. :try_end_2} :catchall_0

    .line 270
    :cond_4
    invoke-direct {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->refresh()V

    .line 271
    iget v2, p0, Lio/github/vitarecomps/frontend/BuildActivity;->step:I

    if-nez v2, :cond_5

    invoke-static {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->needsSetup(Landroid/content/Context;)Z

    move-result v2

    if-nez v2, :cond_5

    goto :goto_4

    :cond_5
    move v3, v1

    :goto_4
    if-eqz v3, :cond_6

    if-nez p1, :cond_6

    .line 273
    invoke-virtual {p0}, Lio/github/vitarecomps/frontend/BuildActivity;->getIntent()Landroid/content/Intent;

    move-result-object p1

    const-string v2, "add_addon"

    invoke-virtual {p1, v2, v1}, Landroid/content/Intent;->getBooleanExtra(Ljava/lang/String;Z)Z

    move-result p1

    if-eqz p1, :cond_6

    .line 274
    invoke-direct {p0, v6}, Lio/github/vitarecomps/frontend/BuildActivity;->pick(I)V

    .line 276
    :cond_6
    new-instance p1, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda10;

    invoke-direct {p1, p0, v3}, Lio/github/vitarecomps/frontend/BuildActivity$$ExternalSyntheticLambda10;-><init>(Lio/github/vitarecomps/frontend/BuildActivity;Z)V

    invoke-virtual {v0, p1}, Landroid/widget/ScrollView;->post(Ljava/lang/Runnable;)Z

    return-void

    :catchall_0
    move-exception p0

    .line 268
    :try_start_3
    monitor-exit v2
    :try_end_3
    .catchall {:try_start_3 .. :try_end_3} :catchall_0

    throw p0
.end method

.method protected onPause()V
    .locals 1

    .line 288
    invoke-super {p0}, Landroid/app/Activity;->onPause()V

    const/4 v0, 0x0

    .line 289
    sput-boolean v0, Lio/github/vitarecomps/frontend/BuildActivity;->visible:Z

    .line 290
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->main:Landroid/os/Handler;

    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->poll:Ljava/lang/Runnable;

    invoke-virtual {v0, p0}, Landroid/os/Handler;->removeCallbacks(Ljava/lang/Runnable;)V

    return-void
.end method

.method protected onResume()V
    .locals 1

    .line 281
    invoke-super {p0}, Landroid/app/Activity;->onResume()V

    const/4 v0, 0x1

    .line 282
    sput-boolean v0, Lio/github/vitarecomps/frontend/BuildActivity;->visible:Z

    .line 283
    iget-object v0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->main:Landroid/os/Handler;

    iget-object p0, p0, Lio/github/vitarecomps/frontend/BuildActivity;->poll:Ljava/lang/Runnable;

    invoke-virtual {v0, p0}, Landroid/os/Handler;->post(Ljava/lang/Runnable;)Z

    return-void
.end method

.method protected onDestroy()V
    .locals 0
    invoke-static {p0}, Lio/github/vitarecomps/frontend/MainInstallProgress;->stop(Landroid/app/Activity;)V
    invoke-super {p0}, Landroid/app/Activity;->onDestroy()V
    return-void
.end method
