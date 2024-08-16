use rayon::prelude::*;
use mpi::traits::*;
use mpi::collective::SystemOperation;
use std::sync::{Arc, Mutex};

#[derive(Clone, Copy, Debug)]
struct Body {
    position: [f64; 3],
    velocity: [f64; 3],
    mass: f64,
}

impl Body {
    fn new(position: [f64; 3], velocity: [f64; 3], mass: f64) -> Self {
        Body { position, velocity, mass }
    }
}

fn main() {
    // Inicializa o ambiente MPI
    let universe = mpi::initialize().unwrap();
    let world = universe.world();
    let rank = world.rank();
    let size = world.size() as usize;

    // Definindo constantes e número de corpos
    const G: f64 = 6.67430e-11; // Constante gravitacional
    const DT: f64 = 0.01;      // Intervalo de tempo
    const N: usize = 100;       // Número de corpos

    // Inicializa os corpos
    let mut bodies = vec![
        Body::new([1.0, 2.0, 3.0], [0.0, 0.0, 0.0], 5.0);
        N
    ];

    // Cada processo trabalha em um subconjunto dos corpos
    let chunk_size = N / size;
    let start = rank as usize * chunk_size;
    let end = if rank as usize == size - 1 { N } else { start + chunk_size };
    let my_bodies = &bodies[start..end];

    // Print do estado inicial das partículas
    println!("Rank {} - Estado inicial das partículas:", rank);
    for body in my_bodies {
        println!("{:?}", body);
    }

    // Usa Arc e Mutex para permitir a atualização concorrente
    let updated_bodies = Arc::new(Mutex::new(vec![Body::new([0.0; 3], [0.0; 3], 0.0); end - start]));

    // Calcula as forças e atualiza as posições e velocidades
    my_bodies.par_iter().enumerate().for_each(|(i, body)| {
        let mut force = [0.0; 3];
        for other_body in bodies.iter() {
            let mut diff = [0.0; 3];
            for j in 0..3 {
                diff[j] = other_body.position[j] - body.position[j];
            }
            let distance = (diff.iter().map(|x| x * x).sum::<f64>()).sqrt();
            let f = G * body.mass * other_body.mass / (distance * distance);
            for j in 0..3 {
                force[j] += f * diff[j] / distance;
            }
        }
        let mut updated_body = *body;
        for j in 0..3 {
            updated_body.velocity[j] += force[j] * DT / body.mass;
            updated_body.position[j] += updated_body.velocity[j] * DT;
        }

        // Atualiza o corpo no Mutex
        let mut updated_bodies = updated_bodies.lock().unwrap();
        updated_bodies[i] = updated_body;
    });

    // Print do estado das partículas após a atualização
    println!("Rank {} - Estado das partículas após atualização:", rank);
    let updated_bodies = updated_bodies.lock().unwrap();
    for body in updated_bodies.iter() {
        println!("{:?}", body);
    }

    // Preparando para enviar posições e velocidades via MPI
    let mut all_positions = vec![0.0; N * 3];
    let mut all_velocities = vec![0.0; N * 3];

    // Preenche os vetores para MPI
    for (i, body) in bodies.iter().enumerate() {
        for j in 0..3 {
            all_positions[i * 3 + j] = body.position[j];
            all_velocities[i * 3 + j] = body.velocity[j];
        }
    }

    // Usa MPI para combinar posições e velocidades entre processos
    let mut global_positions = vec![0.0; N * 3];
    let mut global_velocities = vec![0.0; N * 3];

    world.all_reduce_into(&all_positions[..], &mut global_positions[..], &SystemOperation::sum());
    world.all_reduce_into(&all_velocities[..], &mut global_velocities[..], &SystemOperation::sum());

    // Atualiza as posições e velocidades dos corpos
    for (i, body) in bodies.iter_mut().enumerate() {
        for j in 0..3 {
            body.position[j] = global_positions[i * 3 + j] / size as f64;
            body.velocity[j] = global_velocities[i * 3 + j] / size as f64;
        }
    }

    if rank == 0 {
        // Processamento final ou saída dos resultados
        println!("Simulação concluída.");
        // for body in bodies {
        //     println!(
        //         "Posição: {:?}, Velocidade: {:?}, Massa: {}",
        //         body.position, body.velocity, body.mass
        //     );
        // }
    }
}
