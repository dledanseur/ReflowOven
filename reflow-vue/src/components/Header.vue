<template>
  <div>
    <h2>State: {{state}} ({{solderStateString}})</h2>
    <h2>Profile: {{profile}}</h2>
  </div>
</template>
<script>

import state from '../shared-state'

export default {
  mixins: [state],
  name: 'Header'
}
</script>
