[
    {"имя":"взять", "что":{"арг":1, "тело":
        [
            {"имя":"взять", "что":{"режим":"регистр", "имя":"н"}}, 
            "положить",
            1,
            {"имя":"взять", "что":{"режим":"регистр", "имя":"и"}}, 
            "положить",
            1,
            {"имя":"взять", "что":{"режим":"регистр", "имя":"р"}}, 
            "положить",
            {"имя":"метка", "метка":"цикл"},
            {"имя":"взять", "что":{"режим":"регистр", "имя":"и"}},
            "взять",
            {"имя":"взять", "что":{"режим":"регистр", "имя":"р"}},
            "взять",
            "*",
            {"имя":"взять", "что":{"режим":"регистр", "имя":"р"}}, 
            "положить",
            {"имя":"взять", "что":{"режим":"регистр", "имя":"и"}},
            "взять",
            {"имя":"взять", "что":{"режим":"регистр", "имя":"н"}},
            "взять",
            "=",
            {"имя":"если", "да":"конец"},
            {"имя":"взять", "что":{"режим":"регистр", "имя":"и"}},
            "взять",
            1,
            "+",
            {"имя":"взять", "что":{"режим":"регистр", "имя":"и"}},
            "положить",
            {"имя":"переход", "куда":"цикл"},
            {"имя":"метка", "метка":"конец"},
            {"имя":"взять", "что":{"режим":"регистр", "имя":"р"}},
            "взять",
            "сдвинуть",
            {"имя":"вернуть", "рез":1}
        ]}},
    {"имя":"взять", "что":{"режим":"прямой", "адрес":[{"вид":"все"},{"вид":"ключ", "ключ":"таблицы"},{"вид":"ключ", "ключ":"команды"},{"вид":"ключ", "ключ":"факт"}]}},
    "положить",
    "ввод",
    "факт",
    "вывод"
]












