import axios from 'axios'
import { getApiUrl } from './tools'

let titles = [
  { label: 'Time', type: 'number' },
  { label: 'Expected temperature', type: 'number' },
  { label: 'Mesured temperature', type: 'number' },
  { label: 'Internal temperature', type: 'number' }
]

/**
 * @exports.default {Mixin}
 */
export default {
  data: () => ({
    state: 'Stopped',
    profile: '',
    times: [titles],
    solderState: 0
  }),
  methods: {
    addMesure: function (s) {
      this.times.push(s)
    },
    resetMesures: function () {
      this.times = [titles]
    },
    setState: function (s) {
      this.state = s
    },
    setProfile: function (p) {
      this.profile = p
    },
    pointToData: function (data) {
      return [
        data['millis'],
        data['expectedTemperature'],
        data['temperature'],
        data['internalTemperature']
      ]
    }
  },
  computed: {
    solderStateString: function () {
      switch (this.solderState) {
        case 0:
          return 'Warmup'
        case 1:
          return 'Preheat'
        case 2:
          return 'Soack'
        case 3:
          return 'Solder'
      }
    }
  },
  mounted: function () {
    this.$options.sockets.onmessage = (msg) => {
      let data = JSON.parse(msg.data)

      if (data.type === 'newPoint') {
        var record = this.pointToData(data.data)

        this.addMesure(record)

        this.solderState = data.data.state
      } else if (data.type === 'started') {
        this.resetMesures()
        this.setState('Started')
        this.setProfile(data.data.profile)
      } else if (data.type === 'stopped') {
        this.setState('Stopped')
      } else if (data.type === 'changedProfile') {
        this.profile = data.data.profile
      } else if (data.type === 'state') {
        this.profile = data.data.profile
        this.state = data.data.state
        this.solderState = data.data.solderState
      }
    }

    axios.get(getApiUrl('http', '/points')).then(resp => {
      for (var c of resp.data) {
        this.addMesure(this.pointToData(c))
      }
    })
  }
}
