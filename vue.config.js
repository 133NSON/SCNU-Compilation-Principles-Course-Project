const { defineConfig } = require('@vue/cli-service')
module.exports = defineConfig({
  transpileDependencies: true,
  publicPath: './',
  pluginOptions: {
    electronBuilder: {
      builderOptions: {
        "appId": "xxx",
        "productName": "Zafkiel",
        "copyright": "Copyright © 2023 Benson",
        "directories": {
          "output": "./dist"
        },
        "win": {
          "icon": "./public/logo.ico",
          "target": [
            {
              "target": "nsis",
              "arch": [
                "x64",
                "ia32"
              ]
            }
          ]
        },
        "nsis": {
          "oneClick": false,
          "allowElevation": true,
          "allowToChangeInstallationDirectory": true,
          "installerIcon": "./public/logo.ico",
          "uninstallerIcon": "./public/logo.ico",
          "installerHeaderIcon": "./public/logo.ico",
          "createDesktopShortcut": true,
          "createStartMenuShortcut": true,
          "shortcutName": "Zafkiel",
      },
      extraResources: [
          {from: 'backend\\server.exe', to: 'server.exe'},
          { from: 'backend\\a.exe', to: 'a.exe' },
          { from: 'backend\\b.exe', to: 'b.exe' },
          { from: 'backend\\gcc', to: 'gcc' }
      ]
      }
    }
  }
})