# Ployglot - Translation AI assistant OBS Plugin

<div align="center">

[![GitHub](https://img.shields.io/github/license/obs-ai/obs-polyglot)](https://github.com/obs-ai/obs-polyglot/blob/main/LICENSE)
[![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/obs-ai/obs-polyglot/push.yaml)](https://github.com/obs-ai/obs-polyglot/actions/workflows/push.yaml)
[![Total downloads](https://img.shields.io/github/downloads/obs-ai/obs-polyglot/total)](https://github.com/obs-ai/obs-polyglot/releases)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/obs-ai/obs-polyglot)](https://github.com/obs-ai/obs-polyglot/releases)
[![Discord](https://img.shields.io/discord/1200229425141252116)](https://discord.gg/KbjGU2vvUz)

</div>

## Introduction

Polyglot translation AI plugin allows you to translate text in multiple languages in real-time and locally on your machine. ✅ No GPU required, ✅ no cloud costs, ✅ no network and ✅ no downtime! Privacy first - all data stays on your machine.

"polyglot" is derived from the Greek words: "poly-" meaning "many"and "glōtta" (or "glōssa") meaning "tongue" or "language".

It's using the excellent [CTranslate2](https://github.com/OpenNMT/CTranslate2) project from [OpenNMT](https://opennmt.net/).

If this free plugin has been valuable to you consider adding a ⭐ to this GH repo, subscribing to [my YouTube channel](https://www.youtube.com/@royshilk) where I post updates, and supporting my work: https://github.com/sponsors/royshil

### Usage Tutorial
Watch a short tutorial on how to use and setup Polyglot on your OBS scene.
<div align="center">
  <a href="https://youtu.be/ZMNILPWDkDw" target="_blank">
    <img width="40%" src="https://github-production-user-asset-6210df.s3.amazonaws.com/441170/277700000-629554d9-0f71-42a4-a251-71dec1e0b864.PNG" />
  </a>
  <br/>
  https://youtu.be/ZMNILPWDkDw
</div>

Current Features:
- Translate in real time using an internal HTTP server
- Choice of CTranslate2 model file

Roadmap:
- Translation directly on OBS text sources, e.g. with a filter
- Translation of live stream or recording captions

Check out our other plugins:
- [Background Removal](https://github.com/royshil/obs-backgroundremoval) removes background from webcam without a green screen.
- 🚧 Experimental 🚧 [CleanStream](https://github.com/obs-ai/obs-cleanstream) for real-time filler word (uh,um) and profanity removal from live audio stream
- [URL/API Source](https://github.com/obs-ai/obs-urlsource) that allows fetching live data from an API and displaying it in OBS.
- [LocalVocal](https://github.com/royshil/obs-localvocal) speech AI assistant plugin for real-time transcription (captions), translation and more language functions


## Download
Check out the [latest releases](https://github.com/obs-ai/obs-polyglot/releases) for downloads and install instructions.

### Models
_You need to download a CT2 model for the translation service to work._

Here are download links for models that are compatible with the plugin:
- https://forum.opennmt.net/t/nllb-200-with-ctranslate2/5090 NLLB-200 multilingual model (recommended) ([List of NLLB language codes](https://github.com/facebookresearch/LASER/blob/main/nllb/README.md))
- https://huggingface.co/models?sort=trending&search=ctranslate CTranslate2 models on HuggingFace

Download models options: (get e.g. `model.bin` and SPM = SentencePiece Model `.model` file)
- https://forum.opennmt.net/t/nllb-200-with-ctranslate2/5090 (NLLB)
- https://huggingface.co/JustFrederik/m2m_100_418m_ct2_int8/tree/main (M2M100)
- https://huggingface.co/Rohith04/ct2fast_m2m100_418M/tree/main (M2M100)
- https://huggingface.co/jncraton/m2m100_418M-ct2-int8/tree/main (M2M100)

## Building

The plugin was built and tested on Mac OSX  (Intel & Apple silicon), Windows and Linux.

Start by cloning this repo to a directory of your choice.

Remember to sync and fetch the submodules before building, e.g.
```sh
$ git submodule sync --recursive
$ git update --init --recursive
```

### Mac OSX

Using the CI pipeline scripts, locally you would just call the zsh script. By default this builds a universal binary for both Intel and Apple Silicon. To build for a specific architecture please see `.github/scripts/.build.zsh` for the `-arch` options.

```sh
$ ./.github/scripts/build-macos -c Release
```

#### Install
The above script should succeed and the plugin files (e.g. `obs-urlsource.plugin`) will reside in the `./release/Release` folder off of the root. Copy the `.plugin` file to the OBS directory e.g. `~/Library/Application Support/obs-studio/plugins`.

To get `.pkg` installer file, run for example
```sh
$ ./.github/scripts/package-macos -c Release
```
(Note that maybe the outputs will be in the `Release` folder and not the `install` folder like `pakage-macos` expects, so you will need to rename the folder from `build_x86_64/Release` to `build_x86_64/install`)

### Linux (Ubuntu)

Use the CI scripts again
```sh
$ ./.github/scripts/build-linux.sh
```

Copy the results to the standard OBS folders on Ubuntu
```sh
$ sudo cp -R release/RelWithDebInfo/lib/* /usr/lib/x86_64-linux-gnu/
$ sudo cp -R release/RelWithDebInfo/share/* /usr/share/
```
Note: The official [OBS plugins guide](https://obsproject.com/kb/plugins-guide) recommends adding plugins to the `~/.config/obs-studio/plugins` folder.

### Windows

Use the CI scripts again, for example:

```powershell
> .github/scripts/Build-Windows.ps1 -Target x64 -CMakeGenerator "Visual Studio 17 2022"
```

The build should exist in the `./release` folder off the root. You can manually install the files in the OBS directory.

<!-- #### Building with CUDA support on Windows

To build with CUDA support on Windows, you need to install the CUDA toolkit from NVIDIA. The CUDA toolkit is available for download from [here](https://developer.nvidia.com/cuda-downloads).

After installing the CUDA toolkit, you need to set variables to point CMake to the CUDA toolkit installation directory. For example, if you have installed the CUDA toolkit in `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.4`, you need to set `CUDA_TOOLKIT_ROOT_DIR` to `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.4` and `POLYGLOT_WITH_CUDA` to `ON` when running `.github/scripts/Build-Windows.ps1`.

For example
```powershell
.github/scripts/Build-Windows.ps1 -Target x64 -ExtraCmakeArgs '-D','POLYGLOT_WITH_CUDA=ON','-D',"CUDA_TOOLKIT_ROOT_DIR='C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.2'"
```

You will need to copy a few CUDA .dll files to the location of the plugin .dll for it to run. The required .dll files from CUDA (which are located in the `bin` folder of the CUDA toolkit installation directory) are:

- `cudart64_NN.dll`
- `cublas64_NN.dll`
- `cublasLt64_NN.dll`

where `NN` is the CUDA major version number. For example, if you have installed CUDA 12.2 as in example above, then `NN` is `12`. -->
