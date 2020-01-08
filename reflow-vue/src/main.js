import Vue from 'vue'
import App from './App.vue'
import VueGoogleCharts from 'vue-google-charts'
import VueNativeSock from 'vue-native-websocket'
import { getApiUrl } from './tools'

Vue.use(VueNativeSock, getApiUrl('ws', '/ws'), {
  format: 'json',
  reconnection: true, // (Boolean) whether to reconnect automatically (false)
  reconnectionDelay: 1000
})

Vue.config.productionTip = false
Vue.config.devtools = true
Vue.use(VueGoogleCharts)

new Vue({
  render: h => h(App)
}).$mount('#app')
