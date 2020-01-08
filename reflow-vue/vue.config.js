//const HtmlWebpackPlugin = require('html-webpack-plugin');
const WebpackCdnPlugin = require('webpack-cdn-plugin');

module.exports = {
    configureWebpack: {
      plugins: [
 //       new HtmlWebpackPlugin(),
        new WebpackCdnPlugin({
            modules: [
                {
                    name: 'vue',
                    var: 'Vue',
                    //path: 'dist/vue.runtime.min.js',
                    path: 'dist/vue.js',
                    prodUrl: 'https://cdn.jsdelivr.net/npm/:name@:version/:path'
                },
                {
                    name: 'core-js',
                    var: 'CoreJs',
                    path: 'dist/core-js.min.js',
                    prodUrl: 'https://unpkg.com/core-js-bundle@:version/minified.js'
                },
                {
                    name: 'vue-google-charts',
                    var: 'VueGoogleCharts',
                    path: 'dist/vue-google-charts.browser.min.js',
                    prodUrl: 'https://cdn.jsdelivr.net/npm/:name@:version/:path'
                }
            ]
        })
      ]
    }
  }