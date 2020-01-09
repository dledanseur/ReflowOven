<template>
  <div>
    <h2>State: {{state}} {{solderStateStr}}</h2>
    <h2>Profile: {{profile}}</h2>
  </div>
</template>
<script>

import state from '../shared-state'

export default {
  mixins: [state],
  name: 'Header',
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
