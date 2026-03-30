# Vérifier si le module est activé via Kconfig
if(CONFIG_MON_MODULE)

  # Déclarer une bibliothèque Zephyr
  zephyr_library()

  # Ajouter les fichiers sources à compiler
  zephyr_library_sources(src/mon_module.c)

  # Ajouter le dossier "include" aux chemins de recherche des headers
  zephyr_include_directories(include)

endif()