
extern CONTEXT_LOCAL struct vfs_node_s *cwd;

static inline struct vfs_node_s *vfs_get_cwd()
{
    return CONTEXT_LOCAL_GET(cwd);
}

static inline void vfs_set_cwd(struct vfs_node_s *node)
{
    CONTEXT_LOCAL_SET(cwd, node);
}
