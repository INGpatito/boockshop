with db_cursor() as cur:
        if pending_only:
            cur.execute(
                "SELECT id, full_name, service, vehicle, arrival_date, arrival_time, status "
                "FROM reservations WHERE status = 'pending' OR status IS NULL "
                "ORDER BY created_at DESC;"
            )
        else:
            cur.execute(
                "SELECT id, full_name, service, vehicle, arrival_date, arrival_time, status "
                "FROM reservations ORDER BY created_at DESC;"
            )
        return cur.fetchall()
