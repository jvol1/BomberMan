## Como rodar?

## Instalação
- Linux:
    - Instalando o raylib
        - `sudo apt install build-essential git`
        - `sudo apt install libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev`
        - `git clone https://github.com/raysan5/raylib.git raylib`
        - `cd raylib/src/`
        - `make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED`
        - `sudo make install RAYLIB_LIBTYPE=SHARED`
    - Rodando o jogo
        - `git clone https://github.com/gbrls/exemplo-raylib`
        - `cd exemplo-raylib`
        - `make`
- Windows:
   - Prerequisitos
        - Git
    - Intalando o raylib
        - Fazer download do instalador da [biblioteca](https://raysan5.itch.io/raylib/purchase?popup=1) (baixe o do MINGW) e faça a instalação completa, incluindo o notepad++.
    - Rodando o jogo
        - `git clone https://github.com/gbrls/exemplo-raylib`
        - Abra o notepad++ contido na pasta de instalação do raylib (C://raylib/npp/notepad++.exe)
        - Clique em file, em seguida abra a pasta de exemplo baixada e, por ultimo, abra o arquivo main.c contido na pasta src.
        - Tecle F6, clique na caixa de opções da janela no canto inferior esquerdo, em seguida em `temporary script`. Copie e cole o seguinte código  
        ```
            echo > Setup required Environment
            echo -------------------------------------
            SET RAYLIB_PATH=C:\raylib
            SET COMPILER_PATH=C:\raylib\MinGW\bin
            ENV_SET PATH=$(COMPILER_PATH)
            SET CC=gcc
            SET CFLAGS=$(RAYLIB_PATH)\raylib\src\raylib.rc.data -s -static -O2 -std=c99 -Wall -Iexternal -I$(RAYLIB_PATH)/src -DPLATFORM_DESKTOP
            SET LDFLAGS=-lraylib -lopengl32 -lgdi32 -lwinmm
            cd $(CURRENT_DIRECTORY)
            cd ../bin
            echo
            echo > Clean latest build
            echo ------------------------
            cmd /c IF EXIST $(NAME_PART).exe del /F $(NAME_PART).exe
            echo
            echo > Saving Current File
            echo -------------------------
            npp_save
            echo
            echo > Compile program
            echo -----------------------
            cd $(CURRENT_DIRECTORY)
            $(CC) -o ../bin/$(NAME_PART).exe $(FILE_NAME) $(CFLAGS) $(LDFLAGS)
            echo
            echo > Reset Environment
            echo --------------------------
            ENV_UNSET PATH
            echo
            echo > Execute program
            echo -----------------------
            cd $(CURRENT_DIRECTORY)
            cd ../bin
            cmd /c IF EXIST $(NAME_PART).exe $(NAME_PART).exe
        ```  
        Salve, com o nome que desejar, aperte em ok e pronto! Seu jogo irá compilar!
        - Para abrir o jogo, vá na pasta bin e execute o `main.exe`.
