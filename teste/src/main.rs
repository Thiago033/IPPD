use ggez::{Context, GameResult, event, graphics, conf};
use ggez::conf::Conf;
use mpi::traits::*;
use rayon::prelude::*;

struct MainState;

impl MainState {
    fn new(_ctx: &mut Context) -> GameResult<MainState> {
        let s = MainState {};
        Ok(s)
    }
}

impl event::EventHandler<ggez::GameError> for MainState {
    fn update(&mut self, _ctx: &mut Context) -> GameResult<()> {
        Ok(())
    }

    fn draw(&mut self, ctx: &mut Context) -> GameResult<()> {
        // Clear the screen with a white color
        graphics::clear(ctx, graphics::Color::WHITE);
        graphics::present(ctx)?;
        Ok(())
    }
}

fn main() -> GameResult<()> {
    // Initialize MPI
    let universe = mpi::initialize().unwrap();
    let world = universe.world();
    let size = world.size();
    let rank = world.rank();

    // Print MPI info
    println!("Hello, world, I am process {} of {}.", rank, size);

    // Use Rayon to parallelize a computation
    let numbers: Vec<i32> = (0..100).collect();
    let squared_numbers: Vec<i32> = numbers.par_iter().map(|&x| x * x).collect();
    
    if rank == 0 {
        println!("Squared numbers: {:?}", squared_numbers);
    }

    // Initialize ggez
    let (mut ctx, mut event_loop) = ggez::ContextBuilder::new("hello_ggez", "author")
        .conf(Conf::new())
        .build()?;
        
    let mut state = MainState::new(&mut ctx)?;
    event::run(&mut ctx, &mut event_loop, &mut state)
}
