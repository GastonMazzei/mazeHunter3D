
# Define target
NM=Labyrinthe
BACKUP_DIR=backups

# Produce backup
cp "$NM.h" "$NM.h.backup"
cp "$NM.cc" "$NM.cc.backup"

# Use the legacy files
cp "$BACKUP_DIR/$NM.h" "./$NM.h"
cp "$BACKUP_DIR/$NM.cc" "./$NM.cc"

# Do stuff here
make

# Restore backup
cp "$NM.h.backup" "$NM.h"
cp "$NM.cc.backup" "$NM.cc"
