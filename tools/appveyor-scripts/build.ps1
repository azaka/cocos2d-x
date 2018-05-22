Set-PSDebug -Trace 1
$python = "C:\\Python27\\python.exe"

Write-Host "Set environment"
# gradlew
$env:ANDROID_HOME=$env:APPVEYOR_BUILD_FOLDER + "\..\android-sdk"
$env:ANDROID_NDK_HOME=$env:APPVEYOR_BUILD_FOLDER + "\..\android-ndk-r16b"
# gen-libs
$env:ANDROID_SDK_ROOT=$env:APPVEYOR_BUILD_FOLDER + "\..\android-sdk"
$env:NDK_ROOT=$env:APPVEYOR_BUILD_FOLDER + "\..\android-ndk-r16b"

function PushAndroidArtifacts
{
    # https://www.appveyor.com/docs/packaging-artifacts/
    $root = Resolve-Path app\build\outputs\apk; [IO.Directory]::GetFiles($root.Path, '*.*', 'AllDirectories') | % { Push-AppveyorArtifact $_ -FileName $_.Substring($root.Path.Length + 1) -DeploymentName to-publish }
}


If ($env:build_type -eq "android_cpp_tests") {
    Write-Host "Build tests\cpp-tests"
    Push-Location $env:APPVEYOR_BUILD_FOLDER\tests\cpp-tests\proj.android\
    & ./gradlew build
    if ($lastexitcode -ne 0) {throw}
    PushAndroidArtifacts
    Pop-Location

} elseif ($env:build_type -eq "android_cpp_empty_test") {
    Write-Host "Build tests\cpp-empty-test"
    Push-Location $env:APPVEYOR_BUILD_FOLDER\tests\cpp-empty-test\proj.android\
    & ./gradlew build
    if ($lastexitcode -ne 0) {throw}
    PushAndroidArtifacts
    Pop-Location

} elseif ($env:build_type -eq "android_cocos_new_test") {
    Write-Host "Create new project cocos_new_test"
    & $python -u tools\cocos2d-console\bin\cocos.py --agreement n new -l cpp -p my.pack.qqqq cocos_new_test
    if ($lastexitcode -ne 0) {throw}

    Write-Host "Build cocos_new_test"
    Push-Location $env:APPVEYOR_BUILD_FOLDER\cocos_new_test\proj.android\
    & ./gradlew build
    if ($lastexitcode -ne 0) {throw}
    PushAndroidArtifacts
    Pop-Location
# TODO: uncomment when fixed
# } elseif ($env:build_type -eq "android_gen_libs") {
#     Write-Host "Build cocos gen-libs"
#     & $python -u tools\cocos2d-console\bin\cocos.py gen-libs -p android -m release --ap android-15 --app-abi armeabi-v7a --agreement n
#     if ($lastexitcode -ne 0) {throw}

} elseif ($env:build_type -eq "windows32_cocos_new_test") {
    Write-Host "Create new project windows32_cocos_new_test"
    
    # work with cocos 3.8
    & git clone https://github.com/cocos2d/cocos2d-x
    Push-Location cocos2d-x
    
    & git checkout -b 3.8 12ff8cd2dd1bfdcad78abc5eff494cb8cf0053d8
    & $python download-deps.py -r=yes
    & git submodule update --init

    # cocos 3.8
    & $python -u tools\cocos2d-console\bin\cocos.py new -l cpp -p my.pack.qqqq cocos_new_test
    if ($lastexitcode -ne 0) {throw}
    
    # SS5PlayerForCocos2d-x-1.2.0_SS5.6
    & wget https://github.com/SpriteStudio/SS5PlayerForCocos2d-x/archive/1.2.0_SS5.6.tar.gz -O 1.2.0_SS5.6.tar.gz
    & tar -xf 1.2.0_SS5.6.tar.gz
    
    # https://aka.ms/vs/15/release/vs_community.exe
    & wget https://aka.ms/vs/15/release/vs_community.exe -O vs_community.exe
    & vs_community.exe --layout c:\vs2017layout --add Microsoft.VisualStudio.Product.BuildTools -lang en-US
    & 7z a vs2017layout.7z c:\vs2017layout\
    Push-AppveyorArtifact vs2017layout.7z
    
    if ($lastexitcode -ne 0) {throw}
    
    
    $srcproject = $env:APPVEYOR_BUILD_FOLDER + "\cocos2d-x\SS5PlayerForCocos2d-x-1.2.0_SS5.6\samples\cocos2d-x\basic\*"
    $destdir = $env:APPVEYOR_BUILD_FOLDER + "\cocos2d-x\cocos_new_test\"
    
    # overwrites Classes, Resources folders
    Copy-item -Force -Recurse -Verbose $srcproject -Destination $destdir
    
    # replace HelloWorldScene.cpp with this repo's version
    $srcproject = $env:APPVEYOR_BUILD_FOLDER + "\HelloWorldScene.cpp"
    $destdir = $env:APPVEYOR_BUILD_FOLDER + "\cocos2d-x\cocos_new_test\Classes"
    Copy-item -Force -Recurse -Verbose $srcproject -Destination $destdir
    
    # patch lib SS5Player.cpp
    $srcproject = $env:APPVEYOR_BUILD_FOLDER + "\SS5Player.cpp"
    $destdir = $env:APPVEYOR_BUILD_FOLDER + "\cocos2d-x\cocos_new_test\Classes\SSPlayer"
    Copy-item -Force -Recurse -Verbose $srcproject -Destination $destdir
    
    # replace generated cocos_new_test.vcxproj
    $srcproject = $env:APPVEYOR_BUILD_FOLDER + "\cocos_new_test.vcxproj"
    $destdir = $env:APPVEYOR_BUILD_FOLDER + "\cocos2d-x\cocos_new_test\proj.win32\cocos_new_test.vcxproj"
    Copy-item -Force -Recurse -Verbose $srcproject -Destination $destdir
    
    # cocos 3.8
    & msbuild cocos_new_test\proj.win32\cocos_new_test.sln /t:Build /p:Platform="Win32" /p:Configuration="Release" /m /consoleloggerparameters:"PerformanceSummary;NoSummary"
    if ($lastexitcode -ne 0) {throw}
    
    & 7z a release_win32.7z cocos_new_test\proj.win32\Release.win32\
    #& 7z a release_win32.7z cocos_new_test
    if ($lastexitcode -ne 0) {throw}

    Push-AppveyorArtifact release_win32.7z
    
    if ($lastexitcode -ne 0) {throw}
}
Else {
    & msbuild $env:APPVEYOR_BUILD_FOLDER\build\cocos2d-win32.sln /t:Build /p:Platform="Win32" /p:Configuration="Release" /m /consoleloggerparameters:"PerformanceSummary;NoSummary"

    if ($lastexitcode -ne 0) {throw}
    & 7z a release_win32.7z $env:APPVEYOR_BUILD_FOLDER\build\Release.win32\
    if ($lastexitcode -ne 0) {throw}

    Push-AppveyorArtifact release_win32.7z
}
