package saga.permissions
{
  enum permission
  {
    None      = 0,
    Query     = 1,
    Read      = 2,
    Write     = 4,
    Exec      = 8,
    Owner     = 16,
    All       = 31
  }
  interface permissions : implements saga::async
  {
    // setter / getters
    permissions_allow       (in string           id,
                             in int              perm);
    permissions_deny        (in string           id,
                             in int              perm);
    permissions_check       (in string           id,
                             in int              perm,
                             out bool            value);
    get_owner               (out string          owner);
    get_group               (out string          group);
  }
}
