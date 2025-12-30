@echo off
setlocal enabledelayedexpansion

:: 1. Define o caminho da pasta raiz
set "ROOT_DIR=D:\Course\Imgui\Imgui_Couse_dx12ver-master\code\Externals"

:: 2. Cria a pasta external se não existir
if not exist "%ROOT_DIR%" (
    echo [INFO] Criando pasta raiz: %ROOT_DIR%
    mkdir "%ROOT_DIR%"
)

:: 3. Entra no diretório (o /d muda o disco se necessário)
cd /d "%ROOT_DIR%"

:: 4. Lista de repositórios para processar
set "repos=ocornut/imgui epezent/implot sammycage/plutovg sammycage/plutosvg nothings/stb"

echo ============================================
echo   GERENCIADOR DE REPOSITORIOS (GITHUB CLI)
echo ============================================
echo.

:: 5. Loop através de cada repositório da lista
for %%r in (%repos%) do (
    :: Extrai o nome da pasta (o que vem depois da barra)
    for /f "tokens=2 delims=/" %%n in ("%%r") do set "folder_name=%%n"

    if exist "!folder_name!\" (
        echo [ATUALIZAR] !folder_name! ja existe. Atualizando...
        pushd "!folder_name!"
        git pull
        popd
    ) else (
        echo [CLONAR] !folder_name! nao encontrado. Clonando...
        call gh repo clone %%r
    )
    echo --------------------------------------------
)

echo.
echo [OK] Todos os repositorios estao atualizados em:
echo %CD%
echo.
pause