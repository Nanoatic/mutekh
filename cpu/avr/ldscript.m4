
m4_forloop(i, 0, m4_decr(CONFIG_CPU_AVR_IRQ_COUNT), `

  /* define all irq handler that are not already hard defined  */
  PROVIDE(m4_concat(__irq_entry_, i, ) = __irq_entry_default);

  /* handlers must be reachable with a rcall intruction */
  ASSERT(m4_concat(__irq_entry_, i, ) < 0x800, "address location for user irq handler i is too high");

')

