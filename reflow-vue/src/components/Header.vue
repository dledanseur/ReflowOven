<template>
  <div class="header">
    <h2>State: {{state}} {{solderStateStr}}</h2>
    <h2>
      <div>
        <img src="@/assets/arrow_left.png" @click="previousProfile" />
        Profile: {{profile}}
        <img src="@/assets/arrow_right.png" @click="nextProfile" />
      </div>
      <div>
        <img src="@/assets/play.png" v-if="state==='Stopped'" @click="start" />
        <img src="@/assets/stop.png" v-if="state==='Started'" @click="stop" />
      </div>
    </h2>
  </div>
</template>

<style type="text/css">
  .header img {
    cursor: pointer;
  }
</style>

<script>

import state from '../shared-state'

export default {
  mixins: [state],
  name: 'Header',
  methods: {
    previousProfile () {
      this.$socket.sendObj({ type: 'previousProfile' })
    },
    nextProfile () {
      this.$socket.sendObj({ type: 'nextProfile' })
    },
    start () {
      this.$socket.sendObj({ type: 'start' })
    },
    stop () {
      this.$socket.sendObj({ type: 'stop' })
    }
  },
  computed: {
    solderStateStr: function () {
      if (this.state === 'Started') {
        return `(${this.solderStateString})`
      }
      return ''
    }
  }
}
</script>
