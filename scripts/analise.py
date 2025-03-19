import pandas as pd
import numpy as np

file_path = "benchmark.csv"  # Atualize com o caminho correto
df = pd.read_csv(file_path, delimiter=";")

df.rename(columns=lambda x: x.strip(), inplace=True)

print("Colunas do DataFrame:", df.columns.tolist())

df[["N", "F", "H", "C"]] = df["Instance"].str.extract(r"(\d+)-(\d+)-([\d.]+)-([SML])")

df[["N", "F"]] = df[["N", "F"]].astype(int)
df["H"] = df["H"].astype(float)

ga_time_general = df["GA Time (ms)"].mean()
sa_time_general = df["SA Time (ms)"].mean()

ga_time_by_C = df.groupby("C")["GA Time (ms)"].mean()
sa_time_by_C = df.groupby("C")["SA Time (ms)"].mean()

ga_time_by_N = df.groupby("N")["GA Time (ms)"].mean()
sa_time_by_N = df.groupby("N")["SA Time (ms)"].mean()

ga_time_by_F = df.groupby("F")["GA Time (ms)"].mean()
sa_time_by_F = df.groupby("F")["SA Time (ms)"].mean()

ga_time_by_H = df.groupby("H")["GA Time (ms)"].mean()
sa_time_by_H = df.groupby("H")["SA Time (ms)"].mean()

print("\n### Média de Tempo Geral ###")
print(f"GA: {ga_time_general:.2f} ms")
print(f"SA: {sa_time_general:.2f} ms")

print("\n### Média de Tempo por Setup Class ###")
print("GA:")
print(ga_time_by_C)
print("\nSA:")
print(sa_time_by_C)

print("\n### Média de Tempo por Número de Jobs ###")
print("GA:")
print(ga_time_by_N)
print("\nSA:")
print(sa_time_by_N)

print("\n### Média de Tempo por Número de Famílias ###")
print("GA:")
print(ga_time_by_F)
print("\nSA:")
print(sa_time_by_F)

print("\n### Média de Tempo por Intervalo de Due Date ###")
print("GA:")
print(ga_time_by_H)
print("\nSA:")
print(sa_time_by_H)

df[["EDD + LS Lmax", "GA Lmax", "SA Lmax"]] = df[
    ["EDD + LS Lmax", "GA Lmax", "SA Lmax"]
].astype(int)

df["GA Improvement (%)"] = (
    (
        (abs(df["EDD + LS Lmax"] - df["GA Lmax"]))
        / abs(df["EDD + LS Lmax"]).where(
            df["EDD + LS Lmax"] != 0, 1
        )  # Substitui 0 por 1
    )
    * 100
    * (np.where(df["EDD + LS Lmax"] <= df["GA Lmax"], -1, 1))
)

df["SA Improvement (%)"] = (
    (
        (abs(df["EDD + LS Lmax"] - df["SA Lmax"]))
        / abs(df["EDD + LS Lmax"]).where(
            df["EDD + LS Lmax"] != 0, 1
        )  # Substitui 0 por 1
    )
    * 100
    * (np.where(df["EDD + LS Lmax"] <= df["SA Lmax"], -1, 1))
)


def count_improvement(df, algo):
    return pd.DataFrame(
        {
            "Melhorou": (df[f"{algo} Lmax"] < df["EDD + LS Lmax"]).sum(),
            "Manteve": (df[f"{algo} Lmax"] == df["EDD + LS Lmax"]).sum(),
            "Piorou": (df[f"{algo} Lmax"] > df["EDD + LS Lmax"]).sum(),
        },
        index=[0],
    )


ga_improvement_general = count_improvement(df, "GA")
sa_improvement_general = count_improvement(df, "SA")

ga_improvement_by_C = (
    df.groupby("C").apply(lambda g: count_improvement(g, "GA")).droplevel(-1)
)
sa_improvement_by_C = (
    df.groupby("C").apply(lambda g: count_improvement(g, "SA")).droplevel(-1)
)

ga_improvement_by_N = (
    df.groupby("N").apply(lambda g: count_improvement(g, "GA")).droplevel(-1)
)
sa_improvement_by_N = (
    df.groupby("N").apply(lambda g: count_improvement(g, "SA")).droplevel(-1)
)

ga_improvement_by_F = (
    df.groupby("F").apply(lambda g: count_improvement(g, "GA")).droplevel(-1)
)
sa_improvement_by_F = (
    df.groupby("F").apply(lambda g: count_improvement(g, "SA")).droplevel(-1)
)

ga_improvement_by_H = (
    df.groupby("H").apply(lambda g: count_improvement(g, "GA")).droplevel(-1)
)
sa_improvement_by_H = (
    df.groupby("H").apply(lambda g: count_improvement(g, "SA")).droplevel(-1)
)

ga_mean_improvement_general = df["GA Improvement (%)"].mean()
sa_mean_improvement_general = df["SA Improvement (%)"].mean()

ga_mean_improvement_by_C = df.groupby("C")["GA Improvement (%)"].mean()
sa_mean_improvement_by_C = df.groupby("C")["SA Improvement (%)"].mean()

ga_mean_improvement_by_N = df.groupby("N")["GA Improvement (%)"].mean()
sa_mean_improvement_by_N = df.groupby("N")["SA Improvement (%)"].mean()

ga_mean_improvement_by_F = df.groupby("F")["GA Improvement (%)"].mean()
sa_mean_improvement_by_F = df.groupby("F")["SA Improvement (%)"].mean()

ga_mean_improvement_by_H = df.groupby("H")["GA Improvement (%)"].mean()
sa_mean_improvement_by_H = df.groupby("H")["SA Improvement (%)"].mean()

print("\n### Contagem Geral ###")
print("GA:")
print(ga_improvement_general)
print("\nSA:")
print(sa_improvement_general)

print("\n### Contagem por Setup Class ###")
print("GA:")
print(ga_improvement_by_C)
print("\nSA:")
print(sa_improvement_by_C)

print("\n### Contagem por Número de Jobs ###")
print("GA:")
print(ga_improvement_by_N)
print("\nSA:")
print(sa_improvement_by_N)

print("\n### Contagem por Número de Famílias ###")
print("GA:")
print(ga_improvement_by_F)
print("\nSA:")
print(sa_improvement_by_F)

print("\n### Contagem por Intervalo de Due Date ###")
print("GA:")
print(ga_improvement_by_H)
print("\nSA:")
print(sa_improvement_by_H)

print("\n### Média de Melhoria Percentual Geral ###")
print(f"GA: {ga_mean_improvement_general:.2f}%")
print(f"SA: {sa_mean_improvement_general:.2f}%")

print("\n### Média de Melhoria Percentual por Setup Class ###")
print("GA:")
print(ga_mean_improvement_by_C)
print("\nSA:")
print(sa_mean_improvement_by_C)

print("\n### Média de Melhoria Percentual por Número de Jobs ###")
print("GA:")
print(ga_mean_improvement_by_N)
print("\nSA:")
print(sa_mean_improvement_by_N)

print("\n### Média de Melhoria Percentual por Número de Famílias ###")
print("GA:")
print(ga_mean_improvement_by_F)
print("\nSA:")
print(sa_mean_improvement_by_F)

print("\n### Média de Melhoria Percentual por Intervalo de Due Date ###")
print("GA:")
print(ga_mean_improvement_by_H)
print("\nSA:")
print(sa_mean_improvement_by_H)
