<template>
  <GChart
    type="LineChart"
    :settings="{ packages: ['corechart'] }"
    :data="times"
    :options="chartOptions"
  />
</template>
<script>

import state from '../shared-state'

export default {
  mixins: [state],
  name: 'Graph',
  data: () => ({
    chartOptions: {
      chart: {
        title: 'Soldering temperatures'
      }
    }
  })
}
</script>
